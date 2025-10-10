#ifndef MATHJAXWIDGET_H
#define MATHJAXWIDGET_H

#include "include/T42Qt6MathJaxExport.h"

#include <QWidget>
#include <memory>

namespace NTowel42
{
    class CQt6MathJax;

    namespace Ui
    {
        class CMathJaxWidget;
    }

    class T42QT6MATHJAX_EXPORT CMathJaxWidget : public QWidget
    {
        Q_OBJECT

    public:
        explicit CMathJaxWidget( QWidget *parent = nullptr );
        ~CMathJaxWidget();

        void setEngine( NTowel42::CQt6MathJax *engine );
        void setFormula( const QString &formula );
        void setFormulaAndWait( const QString &formula );
        void clear();

    Q_SIGNALS:
        void sigErrorMessage( const QString &errorMsg );

    public Q_SLOTS:
        void slotSetPixelsPerFormula( int pixelsPerFormula );

    private Q_SLOTS:
        void slotSVGRendered( const QString &tex, const QByteArray &svg );

    private:
        void loadSVG( const QByteArray &svg );
        void updateSVGSize();

    private:
        QString fFormula;
        int fPixelsPerFormula{ 200 };
        std::unique_ptr< Ui::CMathJaxWidget > fImpl{};
        NTowel42::CQt6MathJax *fEngine{ nullptr };
    };
}
#endif   // MAINWINDOW_H
