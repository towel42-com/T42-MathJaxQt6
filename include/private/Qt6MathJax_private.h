#ifndef MATHFORMULAENGINEPRIVATE_H
#define MATHFORMULAENGINEPRIVATE_H

#include <QObject>
#include <QCache>
#include <memory>
#include <QWebEnginePage>
#include <QWebEngineUrlSchemeHandler>
#include <QByteArray>

#include <unordered_map>
#include <optional>
class QWebEngineView;
class QWebEngineLoadingInfo;
class QWebEngineNewWindowRequest;
class QWebChannel;

namespace NTowel42
{
    class CQt6MathJax;
    namespace NPrivate
    {
        class CWebEnginePage_WConsoleLog : public QWebEnginePage
        {
            Q_OBJECT
        public:
            explicit CWebEnginePage_WConsoleLog( QObject *parent = nullptr );

        Q_SIGNALS:
            void sigErrorMessage( const QString &msg );

        protected:
            virtual void javaScriptConsoleMessage( JavaScriptConsoleMessageLevel level, const QString &message, int lineNumber, const QString &sourceID ) override;
        };

        struct SQueuedRequests
        {
            SQueuedRequests( const QString &orig, const std::optional< QString > &cleanedTexCode );
            
            QString fOrig;
            QString fClean;
        };

        class CQt6MathJax : public QObject
        {
            Q_OBJECT
        public:
            CQt6MathJax( NTowel42::CQt6MathJax *parent );

        public:
            ~CQt6MathJax();

            void renderSVG( const QString &code );
            void renderSVG( const QString &texCode, const std::function< void( const QString &tex, const std::optional< QByteArray > &svg ) > &function, const std::function< void( const QString &msg ) > &onErrorMessage );

        public:
            bool checkQueue( const QString &texCode );

            // detect whether a string has already been compiled in the past (i.e., is in cache):
            std::optional< QByteArray > beenCreated( const QString &code ) const;
            void clearCache( const QString &code );
            void addToCache( const SQueuedRequests &queuedRequest, const QByteArray &svg );
            void addToCache( const QString &texCode, const std::optional< QString > &cleanedTexCode, const QByteArray &svg );

            QString errorMessage() const;
            QWebEngineView *webEngineView() const;
            bool setEngineReady() const { return fEngineReady; }

            Q_INVOKABLE void errorMessage( const QVariant &msg );
            Q_INVOKABLE void svgRendered( const QVariant &svgs );
            Q_INVOKABLE void renderingFinished();

        Q_SIGNALS:
            Q_INVOKABLE void sigErrorMessage( const QString &msg );
            Q_INVOKABLE void sigSVGRendered( const QString &tex, const QByteArray &svg );
            Q_INVOKABLE void sigEngineReady( bool aOK );
            Q_INVOKABLE void sigRenderingFinished();

        public Q_SLOTS:
            void slotLoadingChanged( const QWebEngineLoadingInfo &loadingInfo );
            void slotRenderNextInQueue();

        private:
            void setupDebugTracing();
            void setEngineReady( bool aOK );

            QWebEngineView *fView{ nullptr };
            QWebEnginePage *page();

            QString fLastError;
            bool fRunning{ false };
            bool fEngineReady{ false };   // false unless the webengine loads the qrc correctly

            mutable std::unordered_map< QString, QByteArray > fSVGCache;

            std::list< SQueuedRequests > fQueue;
        };
    }
}
#endif   // TEXENGINE_H
