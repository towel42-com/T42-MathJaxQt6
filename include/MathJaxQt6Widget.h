#ifndef MATHJAXWIDGET_H
#define MATHJAXWIDGET_H

#include "T42-MathJaxQt6/include/MathJaxQt6Export.h"

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
    class CMathJaxQt6;

    namespace Ui
    {
        class CMathJaxQt6Widget;
    }

    class T42MATHJAXQT6_EXPORT CMathJaxQt6Widget : public QGroupBox
    {
        Q_OBJECT

    public:
        explicit CMathJaxQt6Widget( QWidget *parent = nullptr );
        explicit CMathJaxQt6Widget( const QString &title, QWidget *parent = nullptr );

        ~CMathJaxQt6Widget();

        void setTitle( const QString &title );
        void setEngine( NTowel42::CMathJaxQt6 *engine );

        void setFormula( const std::optional< QString > &formula );
        void setFormulaAndWait( const QString &formula );
        void clear();

        bool isFormula( const std::optional< QString > &formula ) const;

        void setSubordinateTo( CMathJaxQt6Widget *controllingWidget );
        void setSubordinateTo( const std::list< CMathJaxQt6Widget * > &controllingWidgets );

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
        std::list< CMathJaxQt6Widget * > fControllingWidgets;

        NTowel42::CMathJaxQt6 *fEngine{ nullptr };

        QScrollArea *fScrollArea{ nullptr };
        QSvgWidget *fSVGWidget{ nullptr };
    };
}
#endif   
