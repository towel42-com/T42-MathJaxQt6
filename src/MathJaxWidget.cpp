#include "include/MathJaxWidget.h"
#include "include/Qt6MathJax.h"

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
    CMathJaxWidget::CMathJaxWidget( QWidget *parent ) :
        CMathJaxWidget( {}, parent )
    {
    }

    CMathJaxWidget::CMathJaxWidget( const QString &title, QWidget *parent ) :
        QGroupBox( title, parent ),
        fEngine( nullptr )
    {
        setObjectName( "Towel42_CMathJaxWidget" );
        setBackgroundRole( QPalette::Light );

        QSizePolicy sizePolicy( QSizePolicy::Policy::MinimumExpanding, QSizePolicy::Policy::MinimumExpanding );
        sizePolicy.setHorizontalStretch( 0 );
        sizePolicy.setVerticalStretch( 0 );
        setSizePolicy( sizePolicy );

        auto hLayout = new QHBoxLayout( this );
        hLayout->setContentsMargins( 6, 0, 6, 6 );
        hLayout->setSizeConstraint( QLayout::SetMinimumSize );

        fScrollArea = new QScrollArea( this );
        fScrollBarSize = fScrollArea->style()->pixelMetric( QStyle::PM_ScrollBarExtent );
        fScrollArea->setAlignment( Qt::AlignCenter );

        fScrollArea->setSizePolicy( sizePolicy );

        hLayout->addWidget( fScrollArea );
        fScrollArea->setWidgetResizable( false );

        fSVGWidget = new QSvgWidget( fScrollArea );
        fScrollArea->setWidget( fSVGWidget );
        fSVGWidget->setObjectName( "fSVGWidget" );
        fSVGWidget->setMinimumSize( QSize( 0, 10 ) );
        fSVGWidget->setBackgroundRole( QPalette::Light );

        clear();
        updateMinimumHeight();
    }

    CMathJaxWidget::~CMathJaxWidget()
    {
    }

    void CMathJaxWidget::wheelEvent( QWheelEvent *event )
    {
        if ( event->modifiers() == Qt::ControlModifier )
        {
            setScale( fScale * qBound( 0., qPow( 1.2, event->angleDelta().y() / 240. ), 2. ) );
        }
        else
            QGroupBox::wheelEvent( event );
    }

    void CMathJaxWidget::clear()
    {
        fSVGWidget->load( QByteArray() );
        setVisible( false );
    }

    void CMathJaxWidget::setTitle( const QString &title )
    {
        QGroupBox::setTitle( title );
    }

    bool CMathJaxWidget::isFormula( const std::optional< QString > &formula ) const
    {
        if ( !formula.has_value() )
        {
            return !fFormula.has_value();
        }

        if ( !fFormula.has_value() )
        {
            return !formula.has_value();
        }

        return fFormula.value() == formula.value();
    }

    void CMathJaxWidget::loadSVG( const QByteArray &svg )
    {
        if ( controllersHaveFormula( fFormula ) )
        {
            setVisible( false );
            return;
        }

        fSVGWidget->load( svg );
        if ( !fSVGWidget->renderer()->isValid() )
        {
            emit sigErrorMessage( tr( "Could not load the SVG file" ) );
        }
        else
        {
            if ( fSVGWidget->renderer()->aspectRatioMode() != Qt::AspectRatioMode::KeepAspectRatio )
                fSVGWidget->renderer()->setAspectRatioMode( Qt::AspectRatioMode::KeepAspectRatio );

            setVisible( true );
            autoScale();
        }
    }

    double CMathJaxWidget::numFormulas( const QString &tex )
    {
        auto innerRegex = QRegularExpression( QString( R"__(\\newline)__" ) );
        auto regex = QRegularExpression( QString( R"__((\\newline)+)__" ) );

        auto ii = regex.globalMatch( tex );
        double retVal = 1.0;
        while ( ii.hasNext() )
        {
            retVal += 1.0;
            auto match = ii.next();

            int num = 0;
            auto jj = innerRegex.globalMatch( match.captured() );
            while ( jj.hasNext() )
            {
                jj.next();
                num++;
            }
            retVal += ( ( num - 1 ) * 0.5 );
            if ( match.capturedEnd() == tex.length() )
                retVal -= 1;
        }
        return retVal;
    }

    void CMathJaxWidget::resizeEvent( QResizeEvent * /*event*/ )
    {
        autoScale();
    }

    double computeScale( const QSize &lhs, const QSize &rhs )
    {
        auto widthScale = 1.0 * lhs.width() / rhs.width();
        auto heightScale = 1.0 * lhs.height() / rhs.height();
        auto scale = std::min( widthScale, heightScale );
        return scale;
    };

    double CMathJaxWidget::autoScale()
    {
        if ( !fSVGWidget->renderer()->isValid() )
            return 1.0;

        auto svgSize = idealSVGSize();
        auto scale = computeScale( svgSize, svgDefaultSize() );
        if ( scale < fMinScale )
            scale = fMinScale;

        setScale( scale );
        fScrollArea->verticalScrollBar()->setValue( fScrollArea->verticalScrollBar()->maximum() );
        fScrollArea->horizontalScrollBar()->setValue( 0 );

        update();
        return 1.0;
    }

    void CMathJaxWidget::setScale( double newScale )
    {
        auto svgSize = idealSVGSize();
        auto scale = computeScale( svgSize, svgDefaultSize() );

        auto ratio = newScale / scale;
        svgSize = svgSize * ratio;
        fScale = computeScale( svgSize, svgDefaultSize() );

        fSVGWidget->setFixedSize( svgSize );

        update();
    }
    QSize CMathJaxWidget::minimumSizeHint() const
    {
        return { 100, fPixelHeightPerFormula + fScrollBarSize * 2 };
    }

    void CMathJaxWidget::slotSVGRendered( const QString &formula, const QByteArray &svg )
    {
        if ( !isFormula( formula ) )
            return;
        loadSVG( svg );
    }

    void CMathJaxWidget::slotSetPixelsPerFormula( int pixelsPerFormula )
    {
        fPixelHeightPerFormula = pixelsPerFormula;
        updateMinimumHeight();
        autoScale();
    }

    void CMathJaxWidget::updateMinimumHeight()
    {
        setMinimumSize( 10, fPixelHeightPerFormula + fScrollBarSize * 2 );
    }

    void CMathJaxWidget::slotSetMinScale( double minScale )
    {
        fMinScale = minScale;
        autoScale();
    }

    void CMathJaxWidget::setEngine( NTowel42::CQt6MathJax *engine )
    {
        fEngine = engine;
        connect( fEngine, &NTowel42::CQt6MathJax::sigSVGRendered, this, &CMathJaxWidget::slotSVGRendered );
        connect( fEngine, &NTowel42::CQt6MathJax::sigErrorMessage, this, &CMathJaxWidget::sigErrorMessage );
    }

    void CMathJaxWidget::setSubordinateTo( const std::list< CMathJaxWidget * > &controllingWidgets )
    {
        fControllingWidgets = controllingWidgets;
    }

    void CMathJaxWidget::setSubordinateTo( CMathJaxWidget *controllingWidget )
    {
        setSubordinateTo( std::list< CMathJaxWidget * >( { controllingWidget } ) );
    }

    bool CMathJaxWidget::controllersHaveFormula( const std::optional< QString > &formula ) const
    {
        for ( auto &&ii : fControllingWidgets )
        {
            if ( ii->isFormula( formula ) )
                return true;
        }
        return false;
    }

    void CMathJaxWidget::setFormula( const std::optional< QString > &formula )
    {
        Q_ASSERT( fEngine );

        if ( isFormula( formula ) )
            return;

        if ( controllersHaveFormula( formula ) )
        {
            setVisible( false );
            return;
        }

        fFormula = formula;
        if ( !fFormula.has_value() || fFormula.value().isEmpty() )
            clear();
        else
        {
            setVisible( false );
            fEngine->renderSVG( fFormula.value() );
        }
    }

    void CMathJaxWidget::setFormulaAndWait( const QString &formula )
    {
        Q_ASSERT( fEngine );

        if ( formula.isEmpty() )
            clear();
        else if ( isFormula( formula ) )
            return;

        fFormula = formula;
        QApplication::setOverrideCursor( Qt::WaitCursor );
        QByteArray svgCode;
        fEngine->renderSVG(
            fFormula.value(),   //
            [ = ]( const QString & /*tex*/, const std::optional< QByteArray > &svg )   //
            {
                if ( !svg.has_value() )
                {
                    emit sigErrorMessage( tr( "Error in MathJax Engine: %1" ).arg( fEngine->errorMessage() ) );
                    return;
                }

                loadSVG( svg.value() );
            } );
    }

    int CMathJaxWidget::computePerfectHeight() const
    {
        auto numFormulas = this->numFormulas( fFormula.value() );
        auto perfectHeight = ( fPixelHeightPerFormula * numFormulas ) - 2 * fScrollBarSize;
        return perfectHeight;
    }

    QSize CMathJaxWidget::idealSVGSize() const
    {
        auto parentWidget = fScrollArea->viewport();
        auto parentSize = parentWidget->size();

        auto scaleToSize = QSize( parentSize.width() - 2 * fScrollBarSize, computePerfectHeight() );
        return svgDefaultSize().scaled( scaleToSize, Qt::AspectRatioMode::KeepAspectRatio );
    }

    QSize CMathJaxWidget::svgDefaultSize() const
    {
        return fSVGWidget->renderer()->defaultSize();
    }

}