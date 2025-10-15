#ifndef QT6MATHJAX_H
#define QT6MATHJAX_H

#include "MathJaxQt6Export.h"

#include <QObject>
#include <QLoggingCategory>

class QWebEngineView;

T42MATHJAXQT6_EXPORT Q_DECLARE_LOGGING_CATEGORY( T42MathJaxQt6 );
T42MATHJAXQT6_EXPORT Q_DECLARE_LOGGING_CATEGORY( T42MathJaxQt6Console );
T42MATHJAXQT6_EXPORT Q_DECLARE_LOGGING_CATEGORY( T42MathJaxQt6QRC );
T42MATHJAXQT6_EXPORT Q_DECLARE_LOGGING_CATEGORY( T42MathJaxQt6Debug );
T42MATHJAXQT6_EXPORT Q_DECLARE_LOGGING_CATEGORY( T42MathJaxQt6Widget );

namespace NTowel42
{
    namespace NPrivate
    {
        class CMathJaxQt6;
    }
    class T42MATHJAXQT6_EXPORT CMathJaxQt6 : public QObject
    {
        Q_OBJECT
    public:
        CMathJaxQt6( QObject *parent = nullptr );
        ~CMathJaxQt6();

        // async computation of the svg
        void renderSVG( const QString &texCode );

        // synchronous computation of the svg
        void renderSVG( const QString &texCode, const std::function< void( const QString &tex, const std::optional< QByteArray > &svg ) > &function, const std::function< void( const QString &msg ) > &onErrorMessage = {} );

        // allows for pre-loading of formulas and their SVG
        void addToCache( const QString &texCode, const std::optional< QString > &cleanedTexCode, const QByteArray &svg );

        // detect whether a string has already been compiled in the past (i.e., is in cache):
        bool beenCreated( const QString &texCode ) const;
        void clearCache( const QString &texCode );   // removes the texCode from the cache

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
        void sigSVGRendered( const QString &tex, const QByteArray &svg );

    private:
        NPrivate::CMathJaxQt6 *fImpl{ nullptr };
    };

    T42MATHJAXQT6_EXPORT QString cleanupFormula( QString texCode );
}

#endif
