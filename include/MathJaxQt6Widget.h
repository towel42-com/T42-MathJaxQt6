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

        double scale() const { return fScale; }

        bool autoUpdateMinimumParentSize() const { return fAutoUpdateMinimumParentSize; }
        bool hideEmptyOrInvalid() const { return fHideEmptyOrInvalid; }
        bool autoSizeToParentWidth() const { return fAutoSizeToParentWidth; }
        double minScale() const { return fMinScale; }
        double maxScale() const { return fMaxScale; }
        int numPixelsPerFormula() const { return fNumPixelsPerFormula; }

    public:
        virtual void resizeEvent( QResizeEvent *event ) override;

    Q_SIGNALS:
        void sigErrorMessage( const QString &errorMsg );
        void sigScaleChanged( double scaleValue );

    public Q_SLOTS:
        void slotSetNumPixelsPerFormula( int pixelsPerFormula );
        void slotSetMinScale( double minScale );
        void slotSetMaxScale( double maxScale );
        void slotSetAutoUpdateMinimumParentSize( bool autoUpdateMinimumParentSize );
        void slotHideEmptyOrInvalid( bool hideEmptyOrInvalid );
        void slotSetAutoSizeToParentWidth( bool autoSizeToParentWidth );
        void slotSetScale( double newScale );

    private Q_SLOTS:
        void slotSVGRendered( const QString &tex, const QByteArray &svg );

    private:
        bool showWidget( bool ignoreValid = false );
        int heightPadding() const;
        void setDefaultMinimumSize();
        void autoScale();
        void autoSizeParent( bool force );

        QSize idealSVGSize() const;
        QSize svgDefaultSize() const;

        int computePerfectHeight() const;

        bool controllersHaveFormula( const std::optional< QString > &formula ) const;
        bool controllersHaveFormula() const;
        void loadSVG( const QByteArray &svg );

    private:
        std::optional< QString > fFormula;
        int fScrollBarSize{ 0 };
        int fNumPixelsPerFormula{ 200 };
        double fMinScale{ 0.0125 };
        double fMaxScale{ 10.0 };
        double fScale{ 1.0 };
        bool fAutoUpdateMinimumParentSize{ false };
        bool fHideEmptyOrInvalid{ false };
        bool fAutoSizeToParentWidth{ false };

        std::list< CMathJaxQt6Widget * > fControllingWidgets;
        NTowel42::CMathJaxQt6 *fEngine{ nullptr };

        QScrollArea *fScrollArea{ nullptr };
        QSvgWidget *fSVGWidget{ nullptr };
    };
}

#endif
