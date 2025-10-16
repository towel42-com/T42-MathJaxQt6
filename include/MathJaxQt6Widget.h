#ifndef MATHJAXWIDGET_H
#define MATHJAXWIDGET_H

#include "T42-MathJaxQt6/include/MathJaxQt6Export.h"

#include <QWidget>
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
    class CMathJaxQt6GroupBox;

    class T42MATHJAXQT6_EXPORT CMathJaxQt6Widget : public QWidget
    {
        Q_OBJECT
        Q_PROPERTY( int pixelsPerFormula READ numPixelsPerFormula WRITE slotSetNumPixelsPerFormula )
        Q_PROPERTY( double minScale READ minScale WRITE slotSetMinScale )
        Q_PROPERTY( double maxScale READ maxScale WRITE slotSetMaxScale )
        Q_PROPERTY( double scale READ scale WRITE slotSetScale )
        Q_PROPERTY( bool autoUpdateMinimumParentHeight READ autoUpdateMinimumParentHeight WRITE slotSetAutoUpdateMinimumParentHeight )
        Q_PROPERTY( bool hideEmptyOrInvalid READ hideEmptyOrInvalid WRITE slotHideEmptyOrInvalid )
        Q_PROPERTY( bool autoSizeToParentWidth READ autoSizeToParentWidth WRITE slotSetAutoSizeToParentWidth )

    public:
        explicit CMathJaxQt6Widget( QWidget *parent = nullptr );

        ~CMathJaxQt6Widget();

        std::shared_ptr< NTowel42::CMathJaxQt6 > engine() const;
        std::shared_ptr< NTowel42::CMathJaxQt6 > engine();
        void setEngine( const std::shared_ptr< NTowel42::CMathJaxQt6 > &engine );

        void setFormula( const std::optional< QString > &formula );
        void setFormulaAndWait( const QString &formula );
        void clear();

        bool isFormula( const std::optional< QString > &formula ) const;

        void setSubordinateTo( CMathJaxQt6Widget *controllingWidget );
        void setSubordinateTo( const std::list< CMathJaxQt6Widget * > &controllingWidgets );

        static double numFormulas( const QString &tex );

        double scale() const { return fScale; }

        bool autoUpdateMinimumParentHeight() const { return fAutoUpdateMinimumParentHeight; }
        bool hideEmptyOrInvalid() const { return fHideEmptyOrInvalid; }
        bool autoSizeToParentWidth() const { return fAutoSizeToParentWidth; }
        double minScale() const { return fMinScale; }
        double maxScale() const { return fMaxScale; }
        int numPixelsPerFormula() const { return fNumPixelsPerFormula; }

        bool svgValid() const;

    Q_SIGNALS:
        void sigErrorMessage( const QString &errorMsg );
        void sigScaleChanged( double scaleValue );
        void sigEngineReady( bool aOK );
        void sigSVGRendered( const QString &tex, const QByteArray &svg );

    public Q_SLOTS:
        void slotSetNumPixelsPerFormula( int pixelsPerFormula );
        void slotSetMinScale( double minScale );
        void slotSetMaxScale( double maxScale );
        void slotSetAutoUpdateMinimumParentHeight( bool autoUpdateMinimumParentHeight );
        void slotHideEmptyOrInvalid( bool hideEmptyOrInvalid );
        void slotSetAutoSizeToParentWidth( bool autoSizeToParentWidth );
        void slotSetScale( double newScale );

    private Q_SLOTS:
        void slotSVGRendered( const QString &tex, const QByteArray &svg );

    private:
        virtual void wheelEvent( QWheelEvent *event ) override;
        virtual QSize minimumSizeHint() const override;

        void setScale( double newScale, bool updateParentHeight );
        bool showWidget( bool ignoreValid = false );

        void setMathJaxVisible( bool hide );
        CMathJaxQt6GroupBox * mathJaxGroupBox() const;

        void setupUI();

        int heightPadding() const;
        void setDefaultMinimumHeight();
        void autoScale();
        void autoHeightParent();

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
        bool fAutoUpdateMinimumParentHeight{ false };
        bool fHideEmptyOrInvalid{ false };
        bool fAutoSizeToParentWidth{ false };

        std::list< CMathJaxQt6Widget * > fControllingWidgets;
        mutable std::shared_ptr< NTowel42::CMathJaxQt6 > fEngine;

        QScrollArea *fScrollArea{ nullptr };
        QSvgWidget *fSVGWidget{ nullptr };
    };
}

#endif
