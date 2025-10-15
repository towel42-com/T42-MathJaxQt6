#include "include/MathJaxQt6.h"
#include "include/private/MathJaxQt6_private.h"

#include "SABUtils/FileUtils.h"
#include <QDesktopServices>
#include <QEventLoop>
#include <QMetaEnum>
#include <QTimer>
#include <QUrl>
#include <QWebChannel>
#include <QWebEngineFrame>
#include <QWebEngineLoadingInfo>
#include <QWebEnginePage>
#include <QWebEngineView>
#include <QWebEngineUrlScheme>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QDirIterator>

#include <algorithm>
#include <unordered_map>

Q_LOGGING_CATEGORY( T42MathJaxQt6, "Towel42.MathJaxQt6", QtMsgType::QtInfoMsg )
Q_LOGGING_CATEGORY( T42MathJaxQt6Debug, "Towel42.MathJaxQt6.Debug", QtMsgType::QtDebugMsg )
Q_LOGGING_CATEGORY( T42MathJaxQt6Console, "Towel42.MathJaxQt6.Console", QtMsgType::QtInfoMsg );
Q_LOGGING_CATEGORY( T42MathJaxQt6QRC, "Towel42.MathJaxQt6.QRC", QtMsgType::QtDebugMsg );
Q_LOGGING_CATEGORY( T42MathJaxQt6Widget, "Towel42.MathJaxQt6.Widget", QtMsgType::QtDebugMsg );


#undef qCDebug
#undef qCInfo

#define qCDebug( category, ... ) QT_MESSAGE_LOGGER_COMMON( category, QtDebugMsg ).debug( __VA_ARGS__ ).noquote().nospace()
#define qCInfo( category, ... )  QT_MESSAGE_LOGGER_COMMON( category, QtInfoMsg ).info( __VA_ARGS__ ).noquote().nospace()

namespace NTowel42
{
    static std::unordered_map< QString, QString > gCodeCache;
    QString cleanupFormula( QString texCode )
    {
        auto pos = gCodeCache.find( texCode );
        if ( pos == gCodeCache.end() )
        {
            auto origCode = texCode;
            texCode.replace( "\\", "\\\\" ).replace( "'", "\\'" ).replace( "\n", "\\\n" ).trimmed();
            gCodeCache[ origCode ] = texCode;

            pos = gCodeCache.find( origCode );
            Q_ASSERT( pos != gCodeCache.end() );
        }
        else
            texCode = ( *pos ).second;
        return texCode;
    }

    namespace NPrivate
    {
        SQueuedRequests::SQueuedRequests( const QString &orig, const std::optional< QString > &cleanedTexCode ) :
            fOrig( orig )
        {
            if ( cleanedTexCode.has_value() && !cleanedTexCode.value().isEmpty() )
                fClean = cleanedTexCode.value();
            else
                fClean = cleanupFormula( orig );
        }

        CWebEnginePage_WConsoleLog::CWebEnginePage_WConsoleLog( QObject *parent /*= nullptr*/ ) :
            QWebEnginePage( parent )
        {
        }

        void CWebEnginePage_WConsoleLog::javaScriptConsoleMessage( JavaScriptConsoleMessageLevel level, const QString &message, int lineNumber, const QString &sourceID )
        {
            auto typeString = QString( QMetaEnum::fromType< JavaScriptConsoleMessageLevel >().valueToKey( level ) );
            auto pos = typeString.indexOf( "MessageLevel" );
            if ( pos != -1 )
                typeString = typeString.mid( 0, pos );

            auto msg = tr( "%1: %2:%3 - %4" ).arg( typeString ).arg( sourceID ).arg( lineNumber ).arg( message );

            qCInfo( T42MathJaxQt6Console ) << msg;
            if ( level == JavaScriptConsoleMessageLevel::ErrorMessageLevel )
            {
                emit sigErrorMessage( msg );
            }
        }

        void dumpQRC()
        {
            if ( !T42MathJaxQt6QRC().isDebugEnabled() )
                return;

            auto files = NSABUtils::NFileUtils::dumpResources( true );
            for(auto && ii : files)
            {
                qCDebug( T42MathJaxQt6QRC ) << ii;
            }
        }

        CMathJaxQt6::CMathJaxQt6( NTowel42::CMathJaxQt6 *parent ) :
            QObject( parent )
        {
            dumpQRC();

            QWebEngineUrlScheme qrcScheme( QByteArrayLiteral( "qrc" ) );
            qrcScheme.setFlags( QWebEngineUrlScheme::FetchApiAllowed );
            QWebEngineUrlScheme::registerScheme( qrcScheme );

            fView = new QWebEngineView;
            fView->setContextMenuPolicy( Qt::NoContextMenu );
            fView->setMinimumSize( 200, 200 );

            auto page = new CWebEnginePage_WConsoleLog( fView );
            connect( page, &CWebEnginePage_WConsoleLog::sigErrorMessage, this, &CMathJaxQt6::sigErrorMessage );

            fView->setPage( page );

            auto channel = new QWebChannel( page );
            channel->registerObject( QStringLiteral( "qt6MathJax" ), this );
            page->setWebChannel( channel );

            setupDebugTracing();

            connect(
                page, &QWebEnginePage::loadingChanged,
                [ = ]( const QWebEngineLoadingInfo &loadingInfo )   //
                {
                    qCDebug( T42MathJaxQt6Debug ) << "QWebEnginePage: Loading info:";
                    qCDebug( T42MathJaxQt6Debug ) << "          errorCode: " << loadingInfo.errorCode();
                    qCDebug( T42MathJaxQt6Debug ) << "        errorString: \"" << loadingInfo.errorString() << "\"";
                    //qCDebug( T42MathJaxQt6Debug ) << "         isDownload: " << loadingInfo.isDownload();
                    qCDebug( T42MathJaxQt6Debug ) << "        isErrorPage: " << loadingInfo.isErrorPage();
                    qCDebug( T42MathJaxQt6Debug ) << "    responseHeaders: " << loadingInfo.responseHeaders();
                    qCDebug( T42MathJaxQt6Debug ) << "             status: " << loadingInfo.status();
                    qCDebug( T42MathJaxQt6Debug ) << "                url: " << loadingInfo.url();
                } );

            connect( page, &QWebEnginePage::loadingChanged, this, &CMathJaxQt6::slotLoadingChanged );

            QTimer::singleShot(
                0,
                [ = ]()
                {
                    QString url = "qrc:/MathJaxQt6/MathJaxQt6.html";
                    fView->load( url );
                } );
        }

        CMathJaxQt6::~CMathJaxQt6()
        {
            delete fView;
        }

        void CMathJaxQt6::setupDebugTracing()
        {
            connect( fView, &QWebEngineView::iconChanged, [ = ]( const QIcon &icon ) { qCDebug( T42MathJaxQt6Debug ) << "iconChanged" << icon; } );
            connect( fView, &QWebEngineView::iconUrlChanged, [ = ]( const QUrl &url ) { qCDebug( T42MathJaxQt6Debug ) << "iconUrlChanged: " << url; } );
            connect( fView, &QWebEngineView::loadFinished, [ = ]( bool status ) { qCInfo( T42MathJaxQt6 ) << "Load Finished: " << ( status ? "Successful" : "Failed" ); } );
            connect( fView, &QWebEngineView::loadProgress, [ = ]( int progress ) { qCInfo( T42MathJaxQt6 ) << "Load Progress: " << progress; } );
            connect( fView, &QWebEngineView::loadStarted, [ = ]() { qCInfo( T42MathJaxQt6 ) << "Load Started: "; } );
            connect( fView, &QWebEngineView::pdfPrintingFinished, [ = ]( const QString &filePath, bool success ) { qCDebug( T42MathJaxQt6Debug ) << "PDF Print Finished: " << filePath << success; } );
            connect( fView, &QWebEngineView::printFinished, [ = ]( bool success ) { qCDebug( T42MathJaxQt6Debug ) << "Print Finished: " << success; } );
            connect( fView, &QWebEngineView::printRequested, [ = ]() { qCDebug( T42MathJaxQt6Debug ) << "Print Requested: "; } );
            connect( fView, &QWebEngineView::printRequestedByFrame, [ = ]( QWebEngineFrame frame ) { qCDebug( T42MathJaxQt6Debug ) << "Print Requested by Frame: " << frame.name(); } );
            connect( fView, &QWebEngineView::renderProcessTerminated, [ = ]( QWebEnginePage::RenderProcessTerminationStatus terminationStatus, int exitCode ) { qCDebug( T42MathJaxQt6Debug ) << "renderProcessTerminated: " << terminationStatus << exitCode; } );
            connect( fView, &QWebEngineView::selectionChanged, [ = ]() { qCDebug( T42MathJaxQt6Debug ) << "selectionChanged: "; } );
            connect( fView, &QWebEngineView::titleChanged, [ = ]( const QString &title ) { qCDebug( T42MathJaxQt6Debug ) << "titleChanged: " << title; } );
            connect( fView, &QWebEngineView::urlChanged, [ = ]( const QUrl &url ) { qCDebug( T42MathJaxQt6Debug ) << "url changed: " << url; } );
        }

        QWebEngineView *CMathJaxQt6::webEngineView() const
        {
            return fView;
        }

        std::optional< QByteArray > CMathJaxQt6::beenCreated( const QString &texCode ) const
        {
            auto cleanedFormula = cleanupFormula( texCode );
            auto pos = fSVGCache.find( cleanedFormula );
            if ( pos == fSVGCache.end() )
                pos = fSVGCache.find( texCode );
            if ( pos == fSVGCache.end() )
                return {};
            return ( *pos ).second;
        }

        void CMathJaxQt6::clearCache( const QString &texCode )
        {
            QString cleanedFormula = cleanupFormula( texCode );
            auto pos = fSVGCache.find( cleanedFormula );
            if ( pos == fSVGCache.end() )
                return;
            fSVGCache.erase( pos );
        }

        void CMathJaxQt6::addToCache( const QString &texCode, const std::optional< QString > &cleanedTexCode, const QByteArray &svg )
        {
            addToCache( SQueuedRequests( texCode, cleanedTexCode ), svg );
        }

        void CMathJaxQt6::addToCache( const SQueuedRequests &request, const QByteArray &svg )
        {
            fSVGCache[ request.fClean ] = svg;
            fSVGCache[ request.fOrig ] = svg;
        }

        void CMathJaxQt6::renderSVG( const QString &texCode )
        {
            renderSVG( texCode, {}, {} );
        }

        void CMathJaxQt6::renderSVG( const QString &texCode, const std::function< void( const QString &tex, const std::optional< QByteArray > &svg ) > &postRenderFunction, const std::function< void( const QString &msg ) > &onErrorMessage )
        {
            auto cachedValue = beenCreated( texCode );
            if ( cachedValue.has_value() )
            {
                emit sigSVGRendered( texCode, cachedValue.value() );
                if ( postRenderFunction )
                {
                    postRenderFunction( texCode, cachedValue.value() );
                    parent()->blockSignals( false );
                }
                return;
            }
            if ( checkQueue( texCode ) )
                return;
            fQueue.push_back( SQueuedRequests( texCode, {} ) );

            bool rendered = false;
            if ( postRenderFunction )
            {
                auto lambda = [ =, &rendered ]( const QString &tex, const QByteArray &svg )
                {
                    rendered = true;
                    postRenderFunction( tex, svg );
                    parent()->blockSignals( false );
                };
                connect( this, &CMathJaxQt6::sigSVGRendered, lambda );
                if ( onErrorMessage )
                    connect( this, &CMathJaxQt6::sigErrorMessage, onErrorMessage );
            }

            QTimer::singleShot( 0, this, &CMathJaxQt6::slotRenderNextInQueue );

            if ( postRenderFunction )
            {
                QEventLoop loop;
                QObject::connect( this, &CMathJaxQt6::sigRenderingFinished, &loop, &QEventLoop::quit );
                loop.exec();
                if ( !rendered )
                {
                    postRenderFunction( {}, {} );
                    parent()->blockSignals( false );
                }
            }
        }

        bool CMathJaxQt6::checkQueue( const QString &texCode )
        {
            auto cleanedFormula = cleanupFormula( texCode );
            for ( auto &&ii = fQueue.begin(); ii != fQueue.end(); ++ii )
            {
                if ( ( ( *ii ).fOrig == texCode ) && ( ( *ii ).fClean == cleanedFormula ) )
                {
                    if ( ii == fQueue.begin() )   // currently being processed
                    {
                        qCInfo( T42MathJaxQt6 ) << "Formula: '" << cleanedFormula << "' already in queue and currently being processed.";
                    }
                    else
                    {
                        qCInfo( T42MathJaxQt6 ) << "Formula: '" << cleanedFormula << "' already in queue being moved to the next to be processed.";
                        auto curr = *ii;
                        fQueue.erase( ii );
                        fQueue.insert( std::next( fQueue.begin() ), curr );
                    }
                    return true;
                }
            }
            return false;
        }

        void CMathJaxQt6::slotRenderNextInQueue()
        {
            if ( fQueue.empty() )
                return;

            if ( !setEngineReady() )
            {
                // still loading
                if ( errorMessage().isEmpty() )
                {
                    QTimer::singleShot( 200, this, &CMathJaxQt6::slotRenderNextInQueue );
                }
                return;
            }

            if ( fRunning )
            {
                QTimer::singleShot( 200, this, &CMathJaxQt6::slotRenderNextInQueue );
                return;
            }

            qCInfo( T42MathJaxQt6 ) << "Formulas in Queue: " << fQueue.size();

            fRunning = true;
            fLastError = QString();
            auto cleanedFormula = fQueue.front().fClean;

            auto codeToRun = QString(   //
                                 "renderer = new Renderer();"
                                 "renderer.render('%1');" )
                                 .arg( cleanedFormula );
            page()->runJavaScript( codeToRun );
        }

        void CMathJaxQt6::renderingFinished()
        {
            fRunning = false;
            emit sigRenderingFinished();
            if ( !fQueue.empty() )
                fQueue.pop_front();
            if ( !fQueue.empty() )
                QTimer::singleShot( 0, this, &CMathJaxQt6::slotRenderNextInQueue );
        }

        void CMathJaxQt6::errorMessage( const QVariant &msg )
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

                        Q_ASSERT( tmp == keys.back() );

                        retVal += "\n" + QString( ( keys.size() - 1 ) * 4, ' ' ) + curr + '/' + keys.back();
                        byteArray = byteArray.slice( keys.back().length() + 1 );
                        keys.pop_back();
                    }
                    else
                    {
                        auto keyPos = byteArray.indexOf( QRegularExpression( R"__([\s])__" ) );
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

        void CMathJaxQt6::svgRendered( const QVariant &value )
        {
            qCDebug( T42MathJaxQt6Debug ) << "Variant:" << value;

            // orig has html improperly inside quotes
            auto orig = value.toString();
            auto origEOL = cleanXML( orig );
            auto svg = validateXML( origEOL.toUtf8() );
            if ( svg.has_value() )
            {
                qCDebug( T42MathJaxQt6Debug ) << "SVG:" << svg.value();
                addToCache( fQueue.front(), svg.value() );
                emit sigSVGRendered( fQueue.front().fOrig, svg.value() );
                if ( fQueue.front().fOrig != fQueue.front().fClean )
                    emit sigSVGRendered( fQueue.front().fClean, svg.value() );
            }
            else
            {
                errorMessage( "Problems rendering equation" );
            }
        };

        void CMathJaxQt6::setEngineReady( bool aOK )
        {
            fEngineReady = aOK;
            if ( fEngineReady )
                fLastError.clear();
            emit sigEngineReady( fEngineReady );
        };

        QString CMathJaxQt6::errorMessage() const
        {
            return fLastError;
        }

        void CMathJaxQt6::slotLoadingChanged( const QWebEngineLoadingInfo &loadingInfo )
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
                        setEngineReady( true );
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

        QWebEnginePage *CMathJaxQt6::page()
        {
            return fView ? fView->page() : nullptr;
        }
    }

    CMathJaxQt6::CMathJaxQt6( QObject *parent ) :
        QObject( parent )
    {
        initResources();
        fImpl = new NPrivate::CMathJaxQt6( this );
        connect( fImpl, &NPrivate::CMathJaxQt6::sigEngineReady, this, &CMathJaxQt6::sigEngineReady );
        connect( fImpl, &NPrivate::CMathJaxQt6::sigErrorMessage, this, &CMathJaxQt6::sigErrorMessage );
        connect( fImpl, &NPrivate::CMathJaxQt6::sigSVGRendered, this, &CMathJaxQt6::sigSVGRendered );
    }

    CMathJaxQt6::~CMathJaxQt6()
    {
    }
    void CMathJaxQt6::renderSVG( const QString &texCode )
    {
        fImpl->renderSVG( texCode );
    }

    void CMathJaxQt6::renderSVG( const QString &texCode, const std::function< void( const QString &texCode, const std::optional< QByteArray > &svg ) > &function, const std::function< void( const QString &msg ) > &onErrorMessage )
    {
        blockSignals( true );
        fImpl->renderSVG( texCode, function, onErrorMessage );
    }

    void CMathJaxQt6::slotRenderSVG( const QString &texCode )
    {
        renderSVG( texCode );
    }

    void CMathJaxQt6::addToCache( const QString &texCode, const std::optional< QString > &cleanedTexCode, const QByteArray &svg )
    {
        fImpl->addToCache( texCode, cleanedTexCode, svg );
    }

    bool CMathJaxQt6::beenCreated( const QString &texCode ) const
    {
        return fImpl->beenCreated( texCode ).has_value();
    }

    void CMathJaxQt6::clearCache( const QString &texCode )
    {
        return fImpl->clearCache( texCode );
    }

    QString CMathJaxQt6::errorMessage() const
    {
        return fImpl->errorMessage();
    }

    bool CMathJaxQt6::hasError() const
    {
        return !errorMessage().isEmpty();
    }

    bool CMathJaxQt6::engineReady() const
    {
        return fImpl->setEngineReady();
    }

    QWebEngineView *CMathJaxQt6::webEngineView() const
    {
        return fImpl->webEngineView();
    }

    QWidget *CMathJaxQt6::webEngineViewWidget() const
    {
        return webEngineView();
    }

    void CMathJaxQt6::enableDebugConsole( int port )
    {
        qputenv( "QTWEBENGINE_REMOTE_DEBUGGING", QString::number( port ).toUtf8() );
        QDesktopServices::openUrl( QUrl( "http://127.0.0.1:12345" ) );
    }

    void CMathJaxQt6::initResources()
    {
        Q_INIT_RESOURCE( MathJaxQt6 );

        Q_ASSERT( QFile( ":/MathJaxQt6/MathJaxQt6.html" ).exists() );
        Q_ASSERT( QFile( ":/MathJaxQt6/MathJaxQt6.js" ).exists() );
        Q_ASSERT( QFile( ":/MathJaxQt6/MathJaxQt6Init.js" ).exists() );
        if ( !QFile( ":/MathJaxQt6/MathJaxQt6.html" ).exists() || !QFile( ":/MathJaxQt6/MathJaxQt6.js" ).exists() || !QFile( ":/MathJaxQt6/MathJaxQt6Init.js" ).exists() )
        {
            NPrivate::dumpQRC();
        }
    }
}