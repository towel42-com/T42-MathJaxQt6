#include "Qt6MathJax.h"
#include "private/Qt6MathJax_private.h"

#include <QWebEngineView>
#include <QWebEnginePage>
#include <QWebEngineFrame>
#include <QWebEngineLoadingInfo>
#include <QDesktopServices>

#include <QCoreApplication>
#include <QFile>
#include <QUrl>
#include <QWebChannel>
#include <QTimer>
#include <QMetaEnum>
#include <QRegularExpression>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

Q_LOGGING_CATEGORY( Qt6MathJax, "Qt6MathJax", QtMsgType::QtInfoMsg )
Q_LOGGING_CATEGORY( Qt6MathJaxDebug, "Qt6MathJax.Debug", QtMsgType::QtDebugMsg )
Q_LOGGING_CATEGORY( Qt6MathJaxConsole, "Qt6MathJax.Console", QtMsgType::QtInfoMsg );
Q_LOGGING_CATEGORY( Qt6MathJaxQRC, "Qt6MathJax.QRC", QtMsgType::QtDebugMsg );

CWebEnginePage_WConsoleLog::CWebEnginePage_WConsoleLog( QObject *parent /*= nullptr*/ ) :
    QWebEnginePage( parent )
{
}

void CWebEnginePage_WConsoleLog::javaScriptConsoleMessage( JavaScriptConsoleMessageLevel level, const QString &message, int lineNumber, const QString &sourceID )
{
    auto typeString = QMetaEnum::fromType< JavaScriptConsoleMessageLevel >().valueToKey( level );
    auto msg = tr( "%1: %2:%3 - %4" ).arg( typeString ).arg( sourceID ).arg( lineNumber ).arg( message );

    qCInfo( Qt6MathJaxConsole ).nospace().noquote() << msg;
    if ( level == JavaScriptConsoleMessageLevel::ErrorMessageLevel )
    {
        emit sigErrorMessage( msg );
    }
}

CQt6MathJax_private::CQt6MathJax_private( CQt6MathJax *parent ) :
    QObject( parent )
{
    //qputenv( "QTWEBENGINE_REMOTE_DEBUGGING", "12345" );

    fView = new QWebEngineView;
    fView->setContextMenuPolicy( Qt::NoContextMenu );
    fView->setMinimumSize( 200, 200 );

    auto page = new CWebEnginePage_WConsoleLog( fView );
    connect( page, &CWebEnginePage_WConsoleLog::sigErrorMessage, this, &CQt6MathJax_private::sigErrorMessage );
    fView->setPage( page );
    auto channel = new QWebChannel( page );
    channel->registerObject( QStringLiteral( "qt6MathJax" ), this );
    page->setWebChannel( channel );

    connect( fView, &QWebEngineView::iconChanged, [ = ]( const QIcon &icon ) { qCInfo( Qt6MathJax ) << "QWebEngineView::iconChanged" << icon; } );
    connect( fView, &QWebEngineView::iconUrlChanged, [ = ]( const QUrl &url ) { qCInfo( Qt6MathJax ) << "QWebEngineView::iconUrlChanged: " << url; } );
    connect( fView, &QWebEngineView::loadFinished, [ = ]( bool status ) { qCInfo( Qt6MathJax ) << "QWebEngineView::Load Finished: " << status; } );
    connect( fView, &QWebEngineView::loadProgress, [ = ]( int progress ) { qCInfo( Qt6MathJax ) << "QWebEngineView::Load Progress: " << progress; } );
    connect( fView, &QWebEngineView::loadStarted, [ = ]() { qCInfo( Qt6MathJax ) << "QWebEngineView::Load Started: "; } );
    connect( fView, &QWebEngineView::pdfPrintingFinished, [ = ]( const QString &filePath, bool success ) { qCInfo( Qt6MathJax ) << "QWebEngineView::PDF Print Finished: " << filePath << success; } );
    connect( fView, &QWebEngineView::printFinished, [ = ]( bool success ) { qCInfo( Qt6MathJax ) << "QWebEngineView::Print Finished: " << success; } );
    connect( fView, &QWebEngineView::printRequested, [ = ]() { qCInfo( Qt6MathJax ) << "QWebEngineView::Print Requested: "; } );
    connect( fView, &QWebEngineView::printRequestedByFrame, [ = ]( QWebEngineFrame frame ) { qCInfo( Qt6MathJax ) << "QWebEngineView::Print Requested by Frame: " << frame.name(); } );
    connect( fView, &QWebEngineView::renderProcessTerminated, [ = ]( QWebEnginePage::RenderProcessTerminationStatus terminationStatus, int exitCode ) { qCInfo( Qt6MathJax ) << "QWebEngineView::renderProcessTerminated: " << terminationStatus << exitCode; } );
    connect( fView, &QWebEngineView::selectionChanged, [ = ]() { qCInfo( Qt6MathJax ) << "QWebEngineView::selectionChanged: "; } );
    connect( fView, &QWebEngineView::titleChanged, [ = ]( const QString &title ) { qCInfo( Qt6MathJax ) << "QWebEngineView::titleChanged: " << title; } );
    connect( fView, &QWebEngineView::urlChanged, [ = ]( const QUrl &url ) { qCInfo( Qt6MathJax ) << "QWebEngineView::url changed: " << url; } );

    connect(
        page, &QWebEnginePage::loadingChanged,
        [ = ]( const QWebEngineLoadingInfo &loadingInfo )   //
        {
            qCInfo( Qt6MathJax ).noquote().nospace() << "QWebEnginePage: Loading info:\n";
            qCInfo( Qt6MathJax ).noquote().nospace() << "          errorCode: " << loadingInfo.errorCode();
            qCInfo( Qt6MathJax ).noquote().nospace() << "        errorString: \"" << loadingInfo.errorString() << "\"";
            //qCInfo( Qt6MathJax ).noquote().nospace() << "         isDownload: " << loadingInfo.isDownload();
            qCInfo( Qt6MathJax ).noquote().nospace() << "        isErrorPage: " << loadingInfo.isErrorPage();
            qCInfo( Qt6MathJax ).noquote().nospace() << "    responseHeaders: " << loadingInfo.responseHeaders();
            qCInfo( Qt6MathJax ).noquote().nospace() << "             status: " << loadingInfo.status();
            qCInfo( Qt6MathJax ).noquote().nospace() << "                url: " << loadingInfo.url();
        } );

    connect( page, &QWebEnginePage::loadingChanged, this, &CQt6MathJax_private::slotLoadingChanged );

    QTimer::singleShot(
        0,
        [ = ]()
        {
            QString url = "qrc:/Qt6MathJax/Qt6MathJax.html";
            //QString url = "qrc:/Qt6MathJax/tex2svg-line.html";
            fView->load( url );
        } );
}

CQt6MathJax_private::~CQt6MathJax_private()
{
    delete fView;
}

CQt6MathJax::CQt6MathJax( QObject *parent ) :
    QObject( parent )
{
    fImpl = new CQt6MathJax_private( this );
    connect( fImpl, &CQt6MathJax_private::sigEngineReady, this, &CQt6MathJax::sigEngineReady );
    connect( fImpl, &CQt6MathJax_private::sigErrorMessage, this, &CQt6MathJax::sigErrorMessage );
    connect( fImpl, &CQt6MathJax_private::sigSVGRendered, this, &CQt6MathJax::sigSVGRendered );
}

CQt6MathJax::~CQt6MathJax()
{
}

QWebEngineView *CQt6MathJax_private::webEngineView() const
{
    return fView;
}

std::optional< QByteArray > CQt6MathJax_private::beenCreated( const QString &code ) const
{
    QString cleanedCode = cleanupCode( code );
    auto pos = fSVGCache.find( cleanedCode );
    if ( pos == fSVGCache.end() )
        return {};
    return ( *pos ).second;
}

void CQt6MathJax_private::clearCache( const QString &code )
{
    QString cleanedCode = cleanupCode( code );
    auto pos = fSVGCache.find( cleanedCode );
    if ( pos == fSVGCache.end() )
        return;
    fSVGCache.erase( pos );
}

QString CQt6MathJax_private::cleanupCode( QString code ) const
{
    auto pos = fCodeCache.find( code );
    if ( pos == fCodeCache.end() )
    {
        auto origCode = code;
        code.replace( "\\", "\\\\" ).replace( "'", "\\'" ).replace( "\n", "\\\n" ).trimmed();
        fCodeCache[ origCode ] = code;

        pos = fCodeCache.find( origCode );
        Q_ASSERT( pos != fCodeCache.end() );
    }
    else
        code = ( *pos ).second;
    return code;
}

void CQt6MathJax_private::renderSVG( const QString &code )
{
    auto cachedValue = beenCreated( code );
    if ( cachedValue.has_value() )
    {
        emit sigSVGRendered( cachedValue.value() );
        return;
    }
    fQueue.push_back( code );
    QTimer::singleShot( 20, this, &CQt6MathJax_private::slotComputeNextInQueue );
}

void CQt6MathJax_private::slotComputeNextInQueue()
{
    if ( fQueue.empty() || fRunning )
    {
        QTimer::singleShot( 20, this, &CQt6MathJax_private::slotComputeNextInQueue );
        return;
    }

    auto cleanedCode = cleanupCode( fQueue.front() );

    fRunning = true;
    fLastError = QString();

    page()->runJavaScript( QString( "Render.run('%1');" ).arg( cleanedCode ) );

    QTimer::singleShot( 20, this, &CQt6MathJax_private::slotComputeNextInQueue );
}

void CQt6MathJax_private::renderingFinished()
{
    fRunning = false;
    if ( !fQueue.empty() )
        fQueue.pop_front();
    QTimer::singleShot( 20, this, &CQt6MathJax_private::slotComputeNextInQueue );
}

void CQt6MathJax_private::emitErrorMessage( const QVariant &msg )
{
    fLastError = msg.toString();
    emit sigErrorMessage( fLastError );
}

std::optional< QByteArray > validateXML( const QByteArray &xml )
{
    QByteArray xmlOut;
    QXmlStreamReader reader( xml );
    QXmlStreamWriter writer( &xmlOut );
    writer.setAutoFormatting( true );
    bool invalidElementFound = false;
    while ( !reader.atEnd() )
    {
        reader.readNext();
        if ( reader.tokenType() != QXmlStreamReader::TokenType::Invalid )
        {
            writer.writeCurrentToken( reader );
        }
        else
        {
            invalidElementFound = true;
        }
    }
    if ( !invalidElementFound )
        return xmlOut;
    return {};
}

QString cleanXML( QStringView byteArray )
{
    QString retVal;
    retVal.reserve( byteArray.length() );
    std::list< QString > keys;
    bool inQuote = false;
    for ( ; !byteArray.isEmpty(); byteArray = byteArray.slice( 1 ) )
    {
        auto &&curr = byteArray[ 0 ];
        if ( curr == '"' )
        {
            inQuote = !inQuote;
            retVal += curr;
        }
        else if ( inQuote )
        {
            if ( curr == '<' )
                retVal += "&lt;";
            else if ( curr == '>' )
                retVal += "&gt;";
            else
                retVal += curr;
        }
        else if ( curr == '<' )
        {
            if ( !keys.empty() && ( byteArray.length() > 1 ) && ( byteArray[ 1 ] == '/' ) )
            {
                auto tmp = QStringView( byteArray ).slice( 2, keys.back().length() );
                if ( tmp != keys.back() )
                    int xyz = 0;

                Q_ASSERT( tmp == keys.back() );

                retVal += "\n" + QString( ( keys.size() - 1 ) * 4, ' ' ) + curr + '/' + keys.back();
                byteArray = byteArray.slice( keys.back().length() + 1 );
                keys.pop_back();
            }
            else
            {
                auto keyPos = byteArray.indexOf( QRegularExpression( R"([\s])" ) );
                auto key = byteArray.mid( 1, keyPos - 1 ).toString();
                keys.push_back( key );
                if ( !retVal.isEmpty() )
                    retVal += "\n" + QString( ( keys.size() - 1 ) * 4, ' ' );
                retVal += curr + key;
                byteArray.slice( key.length() );
            }
        }
        else
            retVal += curr;
    }
    return retVal;
}

void CQt6MathJax_private::emitSVGComputed( const QVariant &value )
{
    qCInfo( Qt6MathJaxDebug ).noquote().nospace() << "Variant:" << value;

    // orig has html improperly inside quotes
    auto orig = value.toString();
    auto origEOL = cleanXML( orig );
    auto svg = validateXML( origEOL.toUtf8() );
    if ( svg.has_value() )
    {
        qCInfo( Qt6MathJaxDebug ).noquote().nospace() << "SVG:" << svg.value();
        emit sigSVGRendered( svg.value() );
        fSVGCache[ fQueue.front() ] = svg.value();
    }
    else
        emitErrorMessage( "Problems rendering equation" );
    renderingFinished();
};

void CQt6MathJax_private::emitEngineReady( QVariant aOK )
{
    fEngineReady = aOK.toBool();
    if ( fEngineReady )
        fLastError.clear();
    emit sigEngineReady( fEngineReady );
};

QString CQt6MathJax_private::errorMessage() const
{
    return fLastError;
}

void CQt6MathJax_private::slotLoadingChanged( const QWebEngineLoadingInfo &loadingInfo )
{
    //static bool first = true;
    //if ( first )
    //{
    //    QDesktopServices::openUrl( QUrl( "http://127.0.0.1:12345" ) );
    //    first = false;
    //}

    fEngineReady = false;
    switch ( loadingInfo.status() )
    {
        case QWebEngineLoadingInfo::LoadStartedStatus:
            {
                fLastError.clear();
                return;
            }
            break;
        case QWebEngineLoadingInfo::LoadStoppedStatus:
            {
                fLastError = "Loading stopped by user.";
                emit sigErrorMessage( fLastError );
            }
            break;
        case QWebEngineLoadingInfo::LoadSucceededStatus:
            {
                emitEngineReady( true );
            }
            break;
        case QWebEngineLoadingInfo::LoadFailedStatus:
            {
                fLastError = tr( "Failed to initialize MathJax engine: %1(%2)" ).arg( loadingInfo.errorString() ).arg( loadingInfo.errorCode() );
                emit sigErrorMessage( fLastError );
            }
            break;
    };
}

void CQt6MathJax::renderSVG( const QString &code )
{
    return fImpl->renderSVG( code );
}

bool CQt6MathJax::beenCreated( const QString &code ) const
{
    return fImpl->beenCreated( code ).has_value();
}

void CQt6MathJax::clearCache( const QString &code )
{
    return fImpl->clearCache( code );
}

QString CQt6MathJax::errorMessage() const
{
    return fImpl->errorMessage();
}

bool CQt6MathJax::hasError() const
{
    return !errorMessage().isEmpty();
}

bool CQt6MathJax::engineReady() const
{
    return fImpl->engineReady();
}

QWebEngineView *CQt6MathJax::webEngineView() const
{
    return fImpl->webEngineView();
}

QWidget *CQt6MathJax::webEngineViewWidget() const
{
    return webEngineView();
}

QWebEnginePage *CQt6MathJax_private::page()
{
    return fView ? fView->page() : nullptr;
}
