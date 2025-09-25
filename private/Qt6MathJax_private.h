#ifndef MATHFORMULAENGINEPRIVATE_H
#define MATHFORMULAENGINEPRIVATE_H

#include <QObject>
#include <QCache>
#include <memory>
#include <unordered_map>
#include <optional>
class QWebEngineView;
class CMathFormulaEngine;

class CMathFormulaEngine_Impl : public QObject
{
    Q_OBJECT
public:
    CMathFormulaEngine_Impl( CMathFormulaEngine *parent );
    ~CMathFormulaEngine_Impl();

    // compute immediately, waiting for result (via processEvents()) before returning it:
    QString svg( const QString &code );

    // queue computation for later iff it hasn't yet been done; cache result when done:
    void asyncSVG( const QString &code );

    // detect whether a string has already been compiled in the past (i.e., is in cache):
    std::optional< QString > beenComputed( const QString &code ) const;

    QString error();

    std::shared_ptr< QWebEngineView > webEngineView() const;

    Q_INVOKABLE void MathJaxDone();
    Q_INVOKABLE void MathJaxError( const QString &errorMessage );

public Q_SLOTS:
    void addJSObject();
    void ready( bool loadSucceeded );

private:
    QString cleanupCode( QString code ) const;

    std::shared_ptr< QWebEngineView > fView;

    //QWebFrame *frame;
    QString fLastError;
    bool fRunning{ false };
    bool fIsReady{ false };
    mutable std::unordered_map< QString, QString > fSVGCache;
    mutable std::unordered_map< QString, QString > fCodeCache;

    std::list< QString > fQueue;
    QString fCurrentInput;

    void computeNextInBackground();
    QString computeNow( const QString &code );
};

#endif   // TEXENGINE_H
