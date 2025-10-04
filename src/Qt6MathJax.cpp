#include "include/Qt6MathJax.h"
#include "include/private/Qt6MathJax_private.h"

#include <QDesktopServices>
#include <QEventLoop>
#include <QMetaEnum>
#include <QRegularExpression>
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

Q_LOGGING_CATEGORY( T42Qt6MathJax, "Towel42.Qt6MathJax", QtMsgType::QtInfoMsg )
Q_LOGGING_CATEGORY( T42Qt6MathJaxDebug, "Towel42.Qt6MathJax.Debug", QtMsgType::QtDebugMsg )
Q_LOGGING_CATEGORY( T42Qt6MathJaxConsole, "Towel42.Qt6MathJax.Console", QtMsgType::QtInfoMsg );
Q_LOGGING_CATEGORY( T42Qt6MathJaxQRC, "Towel42.Qt6MathJax.QRC", QtMsgType::QtDebugMsg );

namespace NTowel42
{
    namespace NPrivate
    {
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

            qCInfo( T42Qt6MathJaxConsole ).nospace().noquote() << msg;
            if ( level == JavaScriptConsoleMessageLevel::ErrorMessageLevel )
            {
                emit sigErrorMessage( msg );
            }
        }

        void dumpQRC()
        {
            if ( !T42Qt6MathJaxQRC().isDebugEnabled() )
                return;

            QDirIterator ii( ":", QDirIterator::Subdirectories );
            while ( ii.hasNext() )
            {
                // Advance the iterator to the next entry and print its path.
                auto fileName = ii.next();
                auto size = QFileInfo( fileName ).size();
                qCDebug( T42Qt6MathJaxQRC ).noquote().nospace() << ii.next() << " " << size;
            }
        }

        CQt6MathJax::CQt6MathJax( NTowel42::CQt6MathJax *parent ) :
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
            connect( page, &CWebEnginePage_WConsoleLog::sigErrorMessage, this, &CQt6MathJax::sigErrorMessage );

            fView->setPage( page );

            auto channel = new QWebChannel( page );
            channel->registerObject( QStringLiteral( "qt6MathJax" ), this );
            page->setWebChannel( channel );

            setupDebugTracing();

            connect(
                page, &QWebEnginePage::loadingChanged,
                [ = ]( const QWebEngineLoadingInfo &loadingInfo )   //
                {
                    qCInfo( T42Qt6MathJaxDebug ).noquote().nospace() << "QWebEnginePage: Loading info:";
                    qCInfo( T42Qt6MathJaxDebug ).noquote().nospace() << "          errorCode: " << loadingInfo.errorCode();
                    qCInfo( T42Qt6MathJaxDebug ).noquote().nospace() << "        errorString: \"" << loadingInfo.errorString() << "\"";
                    //qCInfo( T42Qt6MathJaxDebug ).noquote().nospace() << "         isDownload: " << loadingInfo.isDownload();
                    qCInfo( T42Qt6MathJaxDebug ).noquote().nospace() << "        isErrorPage: " << loadingInfo.isErrorPage();
                    qCInfo( T42Qt6MathJaxDebug ).noquote().nospace() << "    responseHeaders: " << loadingInfo.responseHeaders();
                    qCInfo( T42Qt6MathJaxDebug ).noquote().nospace() << "             status: " << loadingInfo.status();
                    qCInfo( T42Qt6MathJaxDebug ).noquote().nospace() << "                url: " << loadingInfo.url();
                } );

            connect( page, &QWebEnginePage::loadingChanged, this, &CQt6MathJax::slotLoadingChanged );

            QTimer::singleShot(
                0,
                [ = ]()
                {
                    QString url = "qrc:/Qt6MathJax/Qt6MathJax.html";
                    fView->load( url );
                } );
        }

        CQt6MathJax::~CQt6MathJax()
        {
            delete fView;
        }

        void CQt6MathJax::setupDebugTracing()
        {
            connect( fView, &QWebEngineView::iconChanged, [ = ]( const QIcon &icon ) { qCInfo( T42Qt6MathJaxDebug ) << "iconChanged" << icon; } );
            connect( fView, &QWebEngineView::iconUrlChanged, [ = ]( const QUrl &url ) { qCInfo( T42Qt6MathJaxDebug ) << "iconUrlChanged: " << url; } );
            connect( fView, &QWebEngineView::loadFinished, [ = ]( bool status ) { qCInfo( T42Qt6MathJax ) << "Load Finished: " << ( status ? "Successful" : "Failed" ); } );
            connect( fView, &QWebEngineView::loadProgress, [ = ]( int progress ) { qCInfo( T42Qt6MathJax ) << "Load Progress: " << progress; } );
            connect( fView, &QWebEngineView::loadStarted, [ = ]() { qCInfo( T42Qt6MathJax ) << "Load Started: "; } );
            connect( fView, &QWebEngineView::pdfPrintingFinished, [ = ]( const QString &filePath, bool success ) { qCInfo( T42Qt6MathJaxDebug ) << "PDF Print Finished: " << filePath << success; } );
            connect( fView, &QWebEngineView::printFinished, [ = ]( bool success ) { qCInfo( T42Qt6MathJaxDebug ) << "Print Finished: " << success; } );
            connect( fView, &QWebEngineView::printRequested, [ = ]() { qCInfo( T42Qt6MathJaxDebug ) << "Print Requested: "; } );
            connect( fView, &QWebEngineView::printRequestedByFrame, [ = ]( QWebEngineFrame frame ) { qCInfo( T42Qt6MathJaxDebug ) << "Print Requested by Frame: " << frame.name(); } );
            connect( fView, &QWebEngineView::renderProcessTerminated, [ = ]( QWebEnginePage::RenderProcessTerminationStatus terminationStatus, int exitCode ) { qCInfo( T42Qt6MathJaxDebug ) << "renderProcessTerminated: " << terminationStatus << exitCode; } );
            connect( fView, &QWebEngineView::selectionChanged, [ = ]() { qCInfo( T42Qt6MathJaxDebug ) << "selectionChanged: "; } );
            connect( fView, &QWebEngineView::titleChanged, [ = ]( const QString &title ) { qCInfo( T42Qt6MathJaxDebug ) << "titleChanged: " << title; } );
            connect( fView, &QWebEngineView::urlChanged, [ = ]( const QUrl &url ) { qCInfo( T42Qt6MathJaxDebug ) << "url changed: " << url; } );
        }

        QWebEngineView *CQt6MathJax::webEngineView() const
        {
            return fView;
        }

        std::optional< QByteArray > CQt6MathJax::beenCreated( const QString &texCode ) const
        {
            QString cleanedCode = cleanupCode( texCode );
            auto pos = fSVGCache.find( cleanedCode );
            if ( pos == fSVGCache.end() )
                pos = fSVGCache.find( texCode );
            if ( pos == fSVGCache.end() )
                return {};
            return ( *pos ).second;
        }

        void CQt6MathJax::clearCache( const QString &texCode )
        {
            QString cleanedCode = cleanupCode( texCode );
            auto pos = fSVGCache.find( cleanedCode );
            if ( pos == fSVGCache.end() )
                return;
            fSVGCache.erase( pos );
        }

        QString CQt6MathJax::cleanupCode( QString texCode ) const
        {
            auto pos = fCodeCache.find( texCode );
            if ( pos == fCodeCache.end() )
            {
                auto origCode = texCode;
                texCode.replace( "\\", "\\\\" ).replace( "'", "\\'" ).replace( "\n", "\\\n" ).trimmed();
                fCodeCache[ origCode ] = texCode;

                pos = fCodeCache.find( origCode );
                Q_ASSERT( pos != fCodeCache.end() );
            }
            else
                texCode = ( *pos ).second;
            return texCode;
        }

        void CQt6MathJax::renderSVG( const QString &texCode )
        {
            renderSVG( texCode, {}, {} );
        }

        void CQt6MathJax::renderSVG( const QString &texCode, const std::function< void( const std::optional< QByteArray > &svg ) > &postRenderFunction, const std::function< void( const QString &msg ) > &onErrorMessage )
        {
            auto cachedValue = beenCreated( texCode );
            if ( cachedValue.has_value() )
            {
                emit sigSVGRendered( texCode, cachedValue.value() );
                if ( postRenderFunction )
                {
                    postRenderFunction( cachedValue.value() );
                    parent()->blockSignals( false );
                }
                return;
            }
            fQueue.push_back( { texCode, cleanupCode( texCode ) } );

            bool rendered = false;
            if ( postRenderFunction )
            {
                auto lambda = [ =, &rendered ]( const QString & /*tex*/, const QByteArray &svg )
                {
                    rendered = true;
                    postRenderFunction( svg );
                    parent()->blockSignals( false );
                };
                connect( this, &CQt6MathJax::sigSVGRendered, lambda );
                if ( onErrorMessage )
                    connect( this, &CQt6MathJax::sigErrorMessage, onErrorMessage );
            }

            QTimer::singleShot( 0, this, &CQt6MathJax::slotRenderNextInQueue );

            if ( postRenderFunction )
            {
                QEventLoop loop;
                QObject::connect( this, &CQt6MathJax::sigRenderingFinished, &loop, &QEventLoop::quit );
                loop.exec();
                if ( !rendered )
                {
                    postRenderFunction( {} );
                    parent()->blockSignals( false );
                }
            }
        }

        void CQt6MathJax::slotRenderNextInQueue()
        {
            if ( fQueue.empty() )
                return;

            if ( !setEngineReady() )
            {
                // still loading
                if ( errorMessage().isEmpty() )
                {
                    QTimer::singleShot( 200, this, &CQt6MathJax::slotRenderNextInQueue );
                }
                return;
            }

            if ( fRunning )
            {
                QTimer::singleShot( 200, this, &CQt6MathJax::slotRenderNextInQueue );
                return;
            }

            fRunning = true;
            fLastError = QString();
            auto cleanedCode = fQueue.front().fClean;

            auto codeToRun = QString(   //
                                 "renderer = new Renderer();"
                                 "renderer.render('%1');" )
                                 .arg( cleanedCode );
            page()->runJavaScript( codeToRun );
        }

        void CQt6MathJax::renderingFinished()
        {
            fRunning = false;
            emit sigRenderingFinished();
            if ( !fQueue.empty() )
                fQueue.pop_front();
            if ( !fQueue.empty() )
                QTimer::singleShot( 0, this, &CQt6MathJax::slotRenderNextInQueue );
        }

        void CQt6MathJax::errorMessage( const QVariant &msg )
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

        void CQt6MathJax::svgRendered( const QVariant &value )
        {
            qCInfo( T42Qt6MathJaxDebug ).noquote().nospace() << "Variant:" << value;

            // orig has html improperly inside quotes
            auto orig = value.toString();
            auto origEOL = cleanXML( orig );
            auto svg = validateXML( origEOL.toUtf8() );
            if ( svg.has_value() )
            {
                qCInfo( T42Qt6MathJaxDebug ).noquote().nospace() << "SVG:" << svg.value();
                fSVGCache[ fQueue.front().fOrig ] = svg.value();
                fSVGCache[ fQueue.front().fClean ] = svg.value();
                emit sigSVGRendered( fQueue.front().fOrig, svg.value() );
                emit sigSVGRendered( fQueue.front().fClean, svg.value() );
            }
            else
            {
                errorMessage( "Problems rendering equation" );
            }
        };

        void CQt6MathJax::setEngineReady( bool aOK )
        {
            fEngineReady = aOK;
            if ( fEngineReady )
                fLastError.clear();
            emit sigEngineReady( fEngineReady );
        };

        QString CQt6MathJax::errorMessage() const
        {
            return fLastError;
        }

        void CQt6MathJax::slotLoadingChanged( const QWebEngineLoadingInfo &loadingInfo )
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

        QWebEnginePage *CQt6MathJax::page()
        {
            return fView ? fView->page() : nullptr;
        }
    }

    CQt6MathJax::CQt6MathJax( QObject *parent ) :
        QObject( parent )
    {
        initResources();
        fImpl = new NPrivate::CQt6MathJax( this );
        connect( fImpl, &NPrivate::CQt6MathJax::sigEngineReady, this, &CQt6MathJax::sigEngineReady );
        connect( fImpl, &NPrivate::CQt6MathJax::sigErrorMessage, this, &CQt6MathJax::sigErrorMessage );
        connect( fImpl, &NPrivate::CQt6MathJax::sigSVGRendered, this, &CQt6MathJax::sigSVGRendered );
    }

    CQt6MathJax::~CQt6MathJax()
    {
    }
    void CQt6MathJax::renderSVG( const QString &texCode )
    {
        fImpl->renderSVG( texCode );
    }

    void CQt6MathJax::renderSVG( const QString &texCode, const std::function< void( const std::optional< QByteArray > &svg ) > &function, const std::function< void( const QString &msg ) > &onErrorMessage )
    {
        blockSignals( true );
        fImpl->renderSVG( texCode, function, onErrorMessage );
    }

    void CQt6MathJax::slotRenderSVG( const QString &texCode )
    {
        renderSVG( texCode );
    }

    bool CQt6MathJax::beenCreated( const QString &texCode ) const
    {
        return fImpl->beenCreated( texCode ).has_value();
    }

    void CQt6MathJax::clearCache( const QString &texCode )
    {
        return fImpl->clearCache( texCode );
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
        return fImpl->setEngineReady();
    }

    QWebEngineView *CQt6MathJax::webEngineView() const
    {
        return fImpl->webEngineView();
    }

    QWidget *CQt6MathJax::webEngineViewWidget() const
    {
        return webEngineView();
    }

    void CQt6MathJax::enableDebugConsole( int port )
    {
        qputenv( "QTWEBENGINE_REMOTE_DEBUGGING", QString::number( port ).toUtf8() );
        QDesktopServices::openUrl( QUrl( "http://127.0.0.1:12345" ) );
    }

    void CQt6MathJax::initResources()
    {
        Q_INIT_RESOURCE( Qt6MathJax );
    }
}