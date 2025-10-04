#ifndef QT6MATHJAX_H
#define QT6MATHJAX_H

#include "T42Qt6MathJaxExport.h"

#include <QObject>
#include <QLoggingCategory>

class QWebEngineView;

T42QT6MATHJAX_EXPORT Q_DECLARE_LOGGING_CATEGORY( T42Qt6MathJax );
T42QT6MATHJAX_EXPORT Q_DECLARE_LOGGING_CATEGORY( T42Qt6MathJaxConsole );
T42QT6MATHJAX_EXPORT Q_DECLARE_LOGGING_CATEGORY( T42Qt6MathJaxQRC );
T42QT6MATHJAX_EXPORT Q_DECLARE_LOGGING_CATEGORY( T42Qt6MathJaxDebug );

namespace NTowel42
{
    namespace NPrivate
    {
        class CQt6MathJax;
    }
    class T42QT6MATHJAX_EXPORT CQt6MathJax : public QObject
    {
        Q_OBJECT
    public:
        CQt6MathJax( QObject *parent = nullptr );
        ~CQt6MathJax();

        // async computation of the svg
        void renderSVG( const QString &texCode );

        // synchronous computation of the svg
        void renderSVG( const QString &texCode, const std::function< void( const std::optional< QByteArray > &svg ) > &function, const std::function< void( const QString &msg ) > &onErrorMessage = {} );

        // detect whether a string has already been compiled in the past (i.e., is in cache):
        bool beenCreated( const QString &texCode ) const;
        void clearCache( const QString &texCode ); // removes the texCode from the cache

        QString errorMessage() const;
        bool hasError() const;

        bool engineReady() const;

        QWebEngineView *webEngineView() const;
        QWidget *webEngineViewWidget() const;

        static void enableDebugConsole( int port );
        static void initResources();
    public Q_SLOTS:
        void slotRenderSVG( const QString &texCode );
    Q_SIGNALS:
        void sigEngineReady( bool aOK );
        void sigErrorMessage( const QString &msg );
        void sigSVGRendered( const QString & tex, const QByteArray &svg );

    private:
        NPrivate::CQt6MathJax *fImpl{ nullptr };
    };
}

#endif
