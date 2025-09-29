#include "Qt6MathJax.h"
#include "private/Qt6MathJax_private.h"

#include <QWebEngineView>
#include <QWebEnginePage>
#include <QWebEngineLoadingInfo>

#include <QCoreApplication>
#include <QFile>
#include <QUrl>
#include <QWebChannel>
#include <QTimer>
#include <QMetaEnum>

Q_LOGGING_CATEGORY( Qt6MathJax, "Qt6MathJax", QtMsgType::QtInfoMsg )
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
    qputenv( "QTWEBENGINE_REMOTE_DEBUGGING", "12345" );

    fView = new QWebEngineView;
    fView->setContextMenuPolicy( Qt::NoContextMenu );

    auto page = new CWebEnginePage_WConsoleLog( fView );
    connect( page, &CWebEnginePage_WConsoleLog::sigErrorMessage, this, &CQt6MathJax_private::sigErrorMessage );
    fPage = page;
    fView->setPage( fPage );
    fChannel = new QWebChannel( fPage );
    fChannel->registerObject( QStringLiteral( "qt6MathJax" ), this );
    fPage->setWebChannel( fChannel );

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
        fPage, &QWebEnginePage::loadingChanged,
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

    connect( fPage, &QWebEnginePage::loadingChanged, this, &CQt6MathJax_private::slotLoadingChanged );

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
    auto pos = fSVGCache.find( cleanupCode( code ) );
    return ( pos != fSVGCache.end() ) ? ( *pos ).second : std::optional< QByteArray >();
}

QString CQt6MathJax_private::cleanupCode( QString code ) const
{
    auto pos = fCodeCache.find( code );
    if ( pos == fCodeCache.end() )
    {
        auto cleanCode = code.replace( "\\", "\\\\" ).replace( "'", "\\'" ).replace( "\n", "\\\n" ).trimmed();
        fCodeCache[ code ] = cleanCode;
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

    fCurrentInput = cleanupCode( fQueue.front() );

    fRunning = true;
    fLastError = QString();
    auto toRun = QString( "Generate.run('%1');" ).arg( fCurrentInput );

    fPage->runJavaScript(
        toRun,
        [ = ]( const QVariant & /*retVal*/ )
        {
            fRunning = false;
            fQueue.pop_front();
            QTimer::singleShot( 20, this, &CQt6MathJax_private::slotComputeNextInQueue );
        } );

    QTimer::singleShot( 20, this, &CQt6MathJax_private::slotComputeNextInQueue );
}

void CQt6MathJax_private::renderingFinished()
{
    fRunning = false;
    if ( !fQueue.empty() )
        fQueue.pop_front();
}

void CQt6MathJax_private::finished()
{
    renderingFinished();
}

void CQt6MathJax_private::finishedWithError( const QString &errorMessage )
{
    renderingFinished();
    fLastError = errorMessage;
    emit sigErrorMessage( fLastError );
}

void CQt6MathJax_private::emitErrorMessage( const QVariant &msg )
{
    emit sigErrorMessage( msg.toString() );
}

void CQt6MathJax_private::emitSVGComputed( const QVariant &value )
{
    qCInfo( Qt6MathJax ).noquote().nospace() << "SVG:" << value;

    auto svg = value.toByteArray();
    qCInfo( Qt6MathJax ).noquote().nospace() << "SVG:" << svg;

    fSVGCache[ fCurrentInput ] = svg;
    emit sigSVGRendered( svg );
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
