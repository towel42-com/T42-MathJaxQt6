#ifndef MATHFORMULAENGINE_H
#define MATHFORMULAENGINE_H

#include <QObject>
#include <QLoggingCategory>

class QWebEngineView;
class CQt6MathJax_private;
Q_DECLARE_LOGGING_CATEGORY( Qt6MathJax )
Q_DECLARE_LOGGING_CATEGORY( Qt6MathJaxConsole )
Q_DECLARE_LOGGING_CATEGORY( Qt6MathJaxQRC )
Q_DECLARE_LOGGING_CATEGORY( Qt6MathJaxDebug )



class CQt6MathJax : public QObject
{
    Q_OBJECT
public:
    CQt6MathJax( QObject *parent = nullptr );
    ~CQt6MathJax();

    // async computation of the svg
    void renderSVG( const QString &code );

    // detect whether a string has already been compiled in the past (i.e., is in cache):
    bool beenCreated( const QString &code ) const;
    void clearCache( const QString &code );

    QString errorMessage() const;
    bool hasError() const;

    bool engineReady() const;


    QWebEngineView * webEngineView() const;
    QWidget *webEngineViewWidget() const;

Q_SIGNALS:
    void sigEngineReady( bool aOK );
    void sigErrorMessage( const QString &msg );
    void sigSVGRendered( const QByteArray &svg );

private:
    CQt6MathJax_private *fImpl{ nullptr };
};

#endif   // TEXENGINE_H
