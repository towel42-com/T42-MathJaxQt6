#ifndef MATHJAXGROUPBOX_H
#define MATHJAXGROUPBOX_H

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
    class CMathJaxQt6Widget;

    namespace Ui
    {
        class CMathJaxQt6GroupBox;
    }

    class T42MATHJAXQT6_EXPORT CMathJaxQt6GroupBox : public QGroupBox
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
        explicit CMathJaxQt6GroupBox( QWidget *parent = nullptr );
        explicit CMathJaxQt6GroupBox( const QString &title, QWidget *parent = nullptr );

        ~CMathJaxQt6GroupBox();

        CMathJaxQt6Widget *mathJaxWidget() const;

        std::shared_ptr< NTowel42::CMathJaxQt6 > engine() const;
        void setEngine( const std::shared_ptr< NTowel42::CMathJaxQt6 > &engine );

        void setFormula( const std::optional< QString > &formula );
        void setFormulaAndWait( const QString &formula );
        void clear();
        bool svgValid() const;

        bool isFormula( const std::optional< QString > &formula ) const;

        void setSubordinateTo( CMathJaxQt6GroupBox *controllingWidget );
        void setSubordinateTo( const std::list< CMathJaxQt6GroupBox * > &controllingWidgets );

        void setSubordinateTo( CMathJaxQt6Widget *controllingWidget );
        void setSubordinateTo( const std::list< CMathJaxQt6Widget * > &controllingWidgets );

        double scale() const;

        bool autoUpdateMinimumParentHeight() const;
        bool hideEmptyOrInvalid() const;
        bool autoSizeToParentWidth() const;
        double minScale() const;
        double maxScale() const;
        int numPixelsPerFormula() const;

        void updateMathJaxWidgetName();

        void setMathJaxVisible( bool visible );
        
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

    private:
        void setupUI();
        CMathJaxQt6Widget *fMathJaxWidget{ nullptr };
    };
}

#endif
