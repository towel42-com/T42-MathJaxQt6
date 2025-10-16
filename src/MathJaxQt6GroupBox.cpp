#include "include/MathJaxQt6GroupBox.h"
#include "include/MathJaxQt6Widget.h"
#include "include/MathJaxQt6.h"

#include <QSvgRenderer>
#include <QRegularExpression>

#include <QHBoxLayout>
#include <QGroupBox>
#include <QSvgWidget>
#include <QApplication>
#include <QScrollArea>
#include <QFrame>
#include <QSizePolicy>
#include <QStyle>
#include <QScrollBar>
#include <QWheelEvent>

namespace NTowel42
{
    CMathJaxQt6GroupBox::CMathJaxQt6GroupBox( QWidget *parent ) :
        CMathJaxQt6GroupBox( {}, parent )
    {
    }

    CMathJaxQt6GroupBox::CMathJaxQt6GroupBox( const QString &title, QWidget *parent ) :
        QGroupBox( title, parent )
    {
        setupUI();
        clear();
    }

    CMathJaxQt6GroupBox::~CMathJaxQt6GroupBox()
    {
    }

    void CMathJaxQt6GroupBox::updateMathJaxWidgetName()
    {
        if ( !title().isEmpty() )
            fMathJaxWidget->setObjectName( title() + "__NTowel42__CMathJaxQt6Widget" );
    }

    void CMathJaxQt6GroupBox::setMathJaxVisible( bool visible )
    {
        setVisible( visible );
    }

    void CMathJaxQt6GroupBox::setupUI()
    {
        setObjectName( "Towel42_CMathJaxGroupBox" );
        setBackgroundRole( QPalette::Light );

        QSizePolicy sizePolicy( QSizePolicy::Policy::MinimumExpanding, QSizePolicy::Policy::MinimumExpanding );
        sizePolicy.setHorizontalStretch( 0 );
        sizePolicy.setVerticalStretch( 0 );
        setSizePolicy( sizePolicy );

        auto hLayout = new QHBoxLayout( this );
        hLayout->setContentsMargins( 6, 0, 6, 6 );
        hLayout->setSizeConstraint( QLayout::SetMinimumSize );

        fMathJaxWidget = new CMathJaxQt6Widget( this );
        updateMathJaxWidgetName();
        hLayout->addWidget( fMathJaxWidget );

        connect( fMathJaxWidget, &CMathJaxQt6Widget::sigErrorMessage, this, &CMathJaxQt6GroupBox::sigErrorMessage );
        connect( fMathJaxWidget, &CMathJaxQt6Widget::sigScaleChanged, this, &CMathJaxQt6GroupBox::sigScaleChanged );
        connect( fMathJaxWidget, &CMathJaxQt6Widget::sigSVGRendered, this, &CMathJaxQt6GroupBox::sigSVGRendered );
        connect( fMathJaxWidget, &CMathJaxQt6Widget::sigEngineReady, this, &CMathJaxQt6GroupBox::sigEngineReady );

        void sigScaleChanged( double scaleValue );

        hLayout->addWidget( fMathJaxWidget );
    }

    CMathJaxQt6Widget *CMathJaxQt6GroupBox::mathJaxWidget() const
    {
        return fMathJaxWidget;
    }

    std::shared_ptr< NTowel42::CMathJaxQt6 > CMathJaxQt6GroupBox::engine() const
    {
        return fMathJaxWidget->engine();
    }

    void CMathJaxQt6GroupBox::clear()
    {
        fMathJaxWidget->clear();
    }

    bool CMathJaxQt6GroupBox::isFormula( const std::optional< QString > &formula ) const
    {
        return fMathJaxWidget->isFormula( formula );
    }

    void CMathJaxQt6GroupBox::slotSetScale( double newScale )
    {
        fMathJaxWidget->slotSetScale( newScale );
    }

    void CMathJaxQt6GroupBox::slotSetAutoSizeToParentWidth( bool autoSizeToParentWidth )
    {
        fMathJaxWidget->slotSetAutoSizeToParentWidth( autoSizeToParentWidth );
    }

    void CMathJaxQt6GroupBox::setEngine( const std::shared_ptr< NTowel42::CMathJaxQt6 > &engine )
    {
        fMathJaxWidget->setEngine( engine );
    }

    void CMathJaxQt6GroupBox::setSubordinateTo( const std::list< CMathJaxQt6GroupBox * > &controllingGroupBoxes )
    {
        std::list< CMathJaxQt6Widget * > widgets;
        for ( auto &&ii : controllingGroupBoxes )
            widgets.push_back( ii->mathJaxWidget() );
        setSubordinateTo( widgets );
    }

    void CMathJaxQt6GroupBox::setSubordinateTo( CMathJaxQt6GroupBox *controllingGroupBox )
    {
        setSubordinateTo( std::list< CMathJaxQt6GroupBox * >( { controllingGroupBox } ) );
    }

    void CMathJaxQt6GroupBox::setSubordinateTo( const std::list< CMathJaxQt6Widget * > &controllingWidgets )
    {
        fMathJaxWidget->setSubordinateTo( controllingWidgets );
    }

    void CMathJaxQt6GroupBox::setSubordinateTo( CMathJaxQt6Widget *controllingWidget )
    {
        setSubordinateTo( std::list< CMathJaxQt6Widget * >( { controllingWidget } ) );
    }

    double CMathJaxQt6GroupBox::scale() const
    {
        return fMathJaxWidget->scale();
    }

    bool CMathJaxQt6GroupBox::autoUpdateMinimumParentHeight() const
    {
        return fMathJaxWidget->autoUpdateMinimumParentHeight();
    }

    bool CMathJaxQt6GroupBox::hideEmptyOrInvalid() const
    {
        return fMathJaxWidget->hideEmptyOrInvalid();
    }

    bool CMathJaxQt6GroupBox::autoSizeToParentWidth() const
    {
        return fMathJaxWidget->autoSizeToParentWidth();
    }

    double CMathJaxQt6GroupBox::minScale() const
    {
        return fMathJaxWidget->minScale();
    }

    double CMathJaxQt6GroupBox::maxScale() const
    {
        return fMathJaxWidget->maxScale();
    }

    int CMathJaxQt6GroupBox::numPixelsPerFormula() const
    {
        return fMathJaxWidget->numPixelsPerFormula();
    }

    void CMathJaxQt6GroupBox::setFormula( const std::optional< QString > &formula )
    {
        fMathJaxWidget->setFormula( formula );
    }

    void CMathJaxQt6GroupBox::setFormulaAndWait( const QString &formula )
    {
        fMathJaxWidget->setFormulaAndWait( formula );
    }

    void CMathJaxQt6GroupBox::slotSetNumPixelsPerFormula( int pixelsPerFormula )
    {
        fMathJaxWidget->slotSetNumPixelsPerFormula( pixelsPerFormula );
    }

    void CMathJaxQt6GroupBox::slotSetMinScale( double minScale )
    {
        fMathJaxWidget->slotSetMinScale( minScale );
    }

    void CMathJaxQt6GroupBox::slotSetMaxScale( double maxScale )
    {
        fMathJaxWidget->slotSetMaxScale( maxScale );
    }

    void CMathJaxQt6GroupBox::slotSetAutoUpdateMinimumParentHeight( bool autoUpdateMinimumParentHeight )
    {
        fMathJaxWidget->slotSetAutoUpdateMinimumParentHeight( autoUpdateMinimumParentHeight );
    }

    void CMathJaxQt6GroupBox::slotHideEmptyOrInvalid( bool hideEmptyOrInvalid )
    {
        fMathJaxWidget->slotHideEmptyOrInvalid( hideEmptyOrInvalid );
    }

    bool CMathJaxQt6GroupBox::svgValid() const
    {
        return fMathJaxWidget->svgValid();
    }
}