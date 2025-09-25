#include "MathFormulaEngine.h"
#include "private/MathFormulaEngine_private.h"

#include <QWebEngineView>
#include <QWebEnginePage>
#include <QCoreApplication>
#include <QFile>
#include <QUrl>

CMathFormulaEngine_Impl::CMathFormulaEngine_Impl( CMathFormulaEngine *parent ) :
    QObject( parent )
{
    fView = std::make_unique< QWebEngineView >();
    connect( fView.get(), &QWebEngineView::loadFinished, this, &CMathFormulaEngine_Impl::ready );
    //QFile fi( ":/main.html" );
    //if ( fi.open( QFile::ReadOnly ) )
    //{
        //auto data = fi.readAll();

        fView->load( QUrl( "qrc:/main.html" ) );
    //}
    //frame = view->page()->mainFrame();
    //connect( frame, SIGNAL( javaScriptWindowObjectCleared() ), this, SLOT( addJSObject() ) );
}

CMathFormulaEngine_Impl::~CMathFormulaEngine_Impl()
{
}

CMathFormulaEngine::CMathFormulaEngine( QObject *parent ) :
    QObject( parent )
{
    fImpl = new CMathFormulaEngine_Impl( this );
}

CMathFormulaEngine::~CMathFormulaEngine()
{
}

//TeXEngine::TeXEngine() :
//    running( false ),
//    fIsReady( false )
//{
//    view = new QWebView( NULL );
//    connect( view, SIGNAL( loadFinished( bool ) ), this, SLOT( ready( bool ) ) );
//    QString toLoad = "qrc:/main.html";
//    view->load( QUrl( toLoad ) );
//    frame = view->page()->mainFrame();
//    connect( frame, SIGNAL( javaScriptWindowObjectCleared() ), this, SLOT( addJSObject() ) );
//    fCache.setMaxCost( 10000000 );
//}

std::shared_ptr< QWebEngineView > CMathFormulaEngine_Impl::webEngineView() const
{
    return fView;
}

std::optional< QString > CMathFormulaEngine_Impl::beenComputed( const QString &code ) const
{
    auto pos = fSVGCache.find( cleanupCode( code ) );
    return ( pos != fSVGCache.end() ) ? ( *pos ).second : std::optional< QString >();
}

QString CMathFormulaEngine_Impl::cleanupCode( QString code ) const
{
    auto pos = fCodeCache.find( code );
    if ( pos == fCodeCache.end() )
    {
        auto cleanCode = code.replace( "\\", "\\\\" ).replace( "'", "\\'" ).replace( "\n", "\\\n" );
        fCodeCache[ code ] = cleanCode;
    }
    else
        code = ( *pos ).second;
    return code;
}

QString CMathFormulaEngine_Impl::svg( const QString &code )
{
    auto cachedValue = beenComputed( code );
    if ( cachedValue.has_value() )
    {
        return cachedValue.value();
    }

    while ( !fIsReady )
    {
        if ( !fLastError.isEmpty() )
        {
            computeNextInBackground();
            return QString();
        }
        QCoreApplication::processEvents( QEventLoop::AllEvents, 20 );
    }

    return computeNow( code );
}

void CMathFormulaEngine_Impl::asyncSVG( const QString &code )
{
    if ( beenComputed( code ) )
        return;
    fQueue.push_back( code );
    computeNextInBackground();
}

void CMathFormulaEngine_Impl::computeNextInBackground()
{
    if ( fQueue.empty() || fRunning )
        return;

    computeNow( fQueue.front() );
}

QString CMathFormulaEngine_Impl::computeNow( const QString &code )
{
    fCurrentInput = cleanupCode( code );
    fRunning = true;
    fLastError = QString();
    QString toRun = QString( "var TeX2SVG_result = null;"
                             "try {"
                             "    TeX2SVG_result = UpdateMath( '%1' );"
                             "} catch ( e ) {"
                             "    TeX2SVG_result = e + '';"
                             "}"
                             "TeX2SVG_result" )
                        .arg( code );

    QVariant result;
    fView->page()->runJavaScript( toRun, [ &result ]( const QVariant &retVal ) { result = retVal; } );
    if ( result.toString() != "started" )
    {
        fLastError = result.toString();
        computeNextInBackground();
        return QString();
    }

    while ( fRunning )
        QCoreApplication::processEvents( QEventLoop::AllEvents, 20 );

    computeNextInBackground();

    auto cachedValue = beenComputed( fCurrentInput );
    if ( cachedValue.has_value() )
        return cachedValue.value();
    return {};
}

void CMathFormulaEngine_Impl::MathJaxDone()
{
    fRunning = false;
    if ( !fQueue.empty() )
        fQueue.pop_front();
    //QWebElementCollection es = frame->findAllElements( "svg" );
    //QString toLoad;
    //QString defs;
    //QRegExp svgre( "<svg([^>]*)>(.*)</svg>" );
    //QRegExp defre( "<defs.*defs>" );
    //foreach( QWebElement e, es )
    //{
    //    QString piece = e.parent().toInnerXml();
    //    if ( defre.indexIn( piece ) > -1 )
    //    {
    //        defs += defre.cap();
    //    }
    //    else if ( svgre.indexIn( piece ) > -1 )
    //    {
    //        QString result = QString( "<svg%1>%2%3</svg>" ).arg( svgre.cap( 1 ) ).arg( defs ).arg( svgre.cap( 2 ) );
    //        fCache.insert( fCurrentInput, new QString( result ), result.count() );
    //    }
    //}
    //computeNextInBackground();
}

void CMathFormulaEngine_Impl::MathJaxError( const QString &errorMessage )
{
    fLastError = errorMessage;
}

QString CMathFormulaEngine_Impl::error()
{
    return fLastError;
}

void CMathFormulaEngine_Impl::addJSObject()
{
    //frame->addToJavaScriptWindowObject( "qtapp", this );
}

void CMathFormulaEngine_Impl::ready( bool loadSucceeded )
{
    if ( loadSucceeded )
    {
        fIsReady = true;
    }
    else
    {
        fLastError = "Failed to load MathJax engine";
    }
    computeNextInBackground();
}

QString CMathFormulaEngine::svg( const QString &code )
{
    return fImpl->svg( code );
}

void CMathFormulaEngine::asyncSVG( const QString &code )
{
    return fImpl->asyncSVG( code );
}

bool CMathFormulaEngine::beenComputed( const QString &code ) const
{
    return fImpl->beenComputed( code ).has_value();
}

QString CMathFormulaEngine::error()
{
    return fImpl->error();
}

std::shared_ptr< QWebEngineView > CMathFormulaEngine::webEngineView() const
{
    return fImpl->webEngineView();
}
