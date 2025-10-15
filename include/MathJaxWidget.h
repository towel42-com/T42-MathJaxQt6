#ifndef MATHJAXWIDGET_H
#define MATHJAXWIDGET_H

#include "T42-Qt6MathJax/include/T42Qt6MathJaxExport.h"

#include <QGroupBox>
#include <QByteArray>
#include <optional>
#include <list>

class QSvgWidget;
class QResizeEvent;
class QScrollArea;
class QWheelEvent;

namespace NTowel42
{
    class CQt6MathJax;

    namespace Ui
    {
        class CMathJaxWidget;
    }

    class T42QT6MATHJAX_EXPORT CMathJaxWidget : public QGroupBox
    {
        Q_OBJECT

    public:
        explicit CMathJaxWidget( QWidget *parent = nullptr );
        explicit CMathJaxWidget( const QString &title, QWidget *parent = nullptr );

        ~CMathJaxWidget();

        void setTitle( const QString &title );
        void setEngine( NTowel42::CQt6MathJax *engine );

        void setFormula( const std::optional< QString > &formula );
        void setFormulaAndWait( const QString &formula );
        void clear();

        bool isFormula( const std::optional< QString > &formula ) const;

        void setSubordinateTo( CMathJaxWidget *controllingWidget );
        void setSubordinateTo( const std::list< CMathJaxWidget * > &controllingWidgets );

        static double numFormulas( const QString &tex );

        virtual void wheelEvent( QWheelEvent *event ) override;

        virtual QSize minimumSizeHint() const override;
        virtual void resizeEvent( QResizeEvent *event ) override;

    Q_SIGNALS:
        void sigErrorMessage( const QString &errorMsg );

    public Q_SLOTS:
        void slotSetPixelsPerFormula( int pixelsPerFormula );
        void slotSetMinScale( double minScale );
        void slotSVGRendered( const QString &tex, const QByteArray &svg );

    private:
        void updateMinimumHeight();
        double autoScale();

        QSize idealSVGSize() const;
        QSize svgDefaultSize() const;

        int computePerfectHeight() const;
        void setScale( double newScale );

        bool controllersHaveFormula( const std::optional< QString > &formula ) const;
        void loadSVG( const QByteArray &svg );

    private:
        std::optional< QString > fFormula;
        int fScrollBarSize{ 0 };
        int fPixelHeightPerFormula{ 200 };
        double fMinScale{ 0.0125 };
        double fScale{ 1.0 };
        std::list< CMathJaxWidget * > fControllingWidgets;

        NTowel42::CQt6MathJax *fEngine{ nullptr };

        QScrollArea *fScrollArea{ nullptr };
        QSvgWidget *fSVGWidget{ nullptr };
    };
}
#endif   
