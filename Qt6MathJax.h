#ifndef MATHFORMULAENGINE_H
#define MATHFORMULAENGINE_H

#include <QObject>
class QWebEngineView;
class CMathFormulaEngine_Impl;

class CMathFormulaEngine : public QObject
{
    Q_OBJECT
public:
    CMathFormulaEngine( QObject *parent = nullptr );
    ~CMathFormulaEngine();

    // compute immediately, waiting for result (via processEvents()) before returning it:
    QString svg( const QString &code );

    // queue computation for later iff it hasn't yet been done; cache result when done:
    void asyncSVG( const QString &code );

    // detect whether a string has already been compiled in the past (i.e., is in cache):
    bool beenComputed( const QString &code ) const;
    QString error();

    std::shared_ptr< QWebEngineView > webEngineView() const;

private:
    CMathFormulaEngine_Impl *fImpl{ nullptr };
};

#endif   // TEXENGINE_H
