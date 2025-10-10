#ifndef MATHJAXWIDGET_H
#define MATHJAXWIDGET_H

#include "T42-Qt6MathJax/include/T42Qt6MathJaxExport.h"

#include <QWidget>
#include <QByteArray>
#include <optional>
#include <memory>
#include <unordered_map>

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
        void setFormula( const std::optional< QString > &formula );
        void setFormulaAndWait( const QString &formula );
        void clear();

        bool isFormula( const std::optional< QString > &formula ) const;
        void updateSVGSize();

        std::optional< QByteArray > svgForFormula( const QString &formula ) const;

        void setSubordinateTo( const std::list< CMathJaxWidget * > &controllingWidgets );
    Q_SIGNALS:
        void sigErrorMessage( const QString &errorMsg );

    public Q_SLOTS:
        void slotSetPixelsPerFormula( int pixelsPerFormula );

    private Q_SLOTS:
        void slotSVGRendered( const QString &tex, const QByteArray &svg );

    private:
        bool controllersHaveFormula( const std::optional< QString > &formula ) const;
        void loadSVG( const QByteArray &svg );

    private:
        std::optional< QString > fFormula;
        int fPixelsPerFormula{ 200 };
        std::list< CMathJaxWidget * > fControllingWidgets;

        NTowel42::CQt6MathJax *fEngine{ nullptr };
        std::unique_ptr< Ui::CMathJaxWidget > fImpl{};

        static std::unordered_map< QString, QByteArray > sFormulaToSVGMap;
    };
}
#endif   // MAINWINDOW_H
