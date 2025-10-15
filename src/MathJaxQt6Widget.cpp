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
    CMathJaxQt6Widget::CMathJaxQt6Widget( QWidget *parent ) :
        CMathJaxQt6Widget( {}, parent )
    {
    }

    CMathJaxQt6Widget::CMathJaxQt6Widget( const QString &title, QWidget *parent ) :
        QGroupBox( title, parent ),
        fEngine( nullptr )
    {
        setupUI();

        clear();
    }

    CMathJaxQt6Widget::~CMathJaxQt6Widget()
    {
    }

    void CMathJaxQt6Widget::wheelEvent( QWheelEvent *event )
    {
        if ( event->modifiers() == Qt::ControlModifier )
        {
            qCDebug( T42MathJaxQt6Widget ) << "WheelEvent:";
            setScale( fScale * qBound( fMinScale, qPow( 1.2, event->angleDelta().y() / 240. ), fMaxScale ), false );
        }
        else
            QGroupBox::wheelEvent( event );
    }

    void CMathJaxQt6Widget::clear()
    {
        fSVGWidget->load( QByteArray() );
        slotSetNumPixelsPerFormula( fNumPixelsPerFormula );
        showWidget();
    }

    void CMathJaxQt6Widget::setTitle( const QString &title )
    {
        QGroupBox::setTitle( title );
    }

    bool CMathJaxQt6Widget::isFormula( const std::optional< QString > &formula ) const
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

    void CMathJaxQt6Widget::loadSVG( const QByteArray &svg )
    {
        showWidget( true );

        fSVGWidget->load( svg );
        if ( !svgValid() )
        {
            emit sigErrorMessage( tr( "Could not load the SVG file" ) );
        }
        else
        {
            if ( fSVGWidget->renderer()->aspectRatioMode() != Qt::AspectRatioMode::KeepAspectRatio )
                fSVGWidget->renderer()->setAspectRatioMode( Qt::AspectRatioMode::KeepAspectRatio );
        }

        if ( showWidget() && fAutoSizeToParentWidth )
            autoScale();
    }

    double CMathJaxQt6Widget::numFormulas( const QString &tex )
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

    double computeScale( const QSize &lhs, const QSize &rhs )
    {
        auto widthScale = 1.0 * lhs.width() / rhs.width();
        auto heightScale = 1.0 * lhs.height() / rhs.height();
        auto scale = std::min( widthScale, heightScale );
        return scale;
    };

    void CMathJaxQt6Widget::autoScale()
    {
        if ( !svgValid() )
            return;

        auto svgSize = idealSVGSize();
        auto scale = computeScale( svgSize, svgDefaultSize() );
        qCDebug( T42MathJaxQt6Widget ) << "Auto Ideal Scale: " << scale;
        if ( scale < fMinScale )
        {
            qCDebug( T42MathJaxQt6Widget ) << "Using minimum scale: " << fMinScale;
            scale = fMinScale;
        }

        setScale( scale, true );

        fScrollArea->verticalScrollBar()->setValue( fScrollArea->verticalScrollBar()->maximum() );
        fScrollArea->horizontalScrollBar()->setValue( 0 );
    }

    void CMathJaxQt6Widget::autoSizeParent()
    {
        if ( fAutoUpdateMinimumParentSize )
        {
            qCDebug( T42MathJaxQt6Widget ) << "==============================";
            qCDebug( T42MathJaxQt6Widget ) << "Updating Parent Minimum Height";

            auto svgHeight = fSVGWidget->size().height();
            qCDebug( T42MathJaxQt6Widget ) << "SVG Height: " << svgHeight;
            auto parentHeight = svgHeight + ( fScrollBarSize * 3 );
            qCDebug( T42MathJaxQt6Widget ) << "New Parent Height: " << parentHeight;

            setMinimumSize( 10, parentHeight );
        }
    }

    void CMathJaxQt6Widget::slotSetScale( double newScale )
    {
        setScale( newScale, true );
    }

    void CMathJaxQt6Widget::setScale( double newScale, bool updateParentSize )
    {
        qCDebug( T42MathJaxQt6Widget ) << "=====================================";
        qCDebug( T42MathJaxQt6Widget ) << "Changing Scale to: " << newScale;

        auto svgSize = idealSVGSize();
        auto scale = computeScale( svgSize, svgDefaultSize() );

        qCDebug( T42MathJaxQt6Widget ) << "SVG Ideal Size: " << svgSize;
        qCDebug( T42MathJaxQt6Widget ) << "SVG Default Size: " << svgDefaultSize();
        qCDebug( T42MathJaxQt6Widget ) << "Ideal Scale: " << scale;

        auto ratio = newScale / scale;
        svgSize = svgSize * ratio;
        qCDebug( T42MathJaxQt6Widget ) << "Current SVG Size: " << fSVGWidget->size();
        qCDebug( T42MathJaxQt6Widget ) << "SVG Size at new Scale: " << svgSize;
        qCDebug( T42MathJaxQt6Widget ) << "Widgets Minimum Size: " << minimumSize();

        fScale = computeScale( svgSize, svgDefaultSize() );
        emit sigScaleChanged( fScale );

        fSVGWidget->setFixedSize( svgSize );

        if ( updateParentSize )
            autoSizeParent();

        update();
    }

    void CMathJaxQt6Widget::slotSetAutoSizeToParentWidth( bool autoSizeToParentWidth )
    {
        fAutoSizeToParentWidth = autoSizeToParentWidth;
        if ( fAutoSizeToParentWidth )
            autoScale();
    }

    QSize CMathJaxQt6Widget::minimumSizeHint() const
    {
        return { 100, fNumPixelsPerFormula + heightPadding() };
    }

    int CMathJaxQt6Widget::heightPadding() const
    {
        return ( fScrollBarSize * 2.5 );
    }

    void CMathJaxQt6Widget::slotSVGRendered( const QString &formula, const QByteArray &svg )
    {
        if ( !isFormula( formula ) )
            return;
        loadSVG( svg );
    }

    void CMathJaxQt6Widget::setEngine( NTowel42::CMathJaxQt6 *engine )
    {
        fEngine = engine;
        connect( fEngine, &NTowel42::CMathJaxQt6::sigSVGRendered, this, &CMathJaxQt6Widget::slotSVGRendered );
        connect( fEngine, &NTowel42::CMathJaxQt6::sigErrorMessage, this, &CMathJaxQt6Widget::sigErrorMessage );
    }

    void CMathJaxQt6Widget::setSubordinateTo( const std::list< CMathJaxQt6Widget * > &controllingWidgets )
    {
        fControllingWidgets = controllingWidgets;
    }

    void CMathJaxQt6Widget::setSubordinateTo( CMathJaxQt6Widget *controllingWidget )
    {
        setSubordinateTo( std::list< CMathJaxQt6Widget * >( { controllingWidget } ) );
    }

    bool CMathJaxQt6Widget::controllersHaveFormula() const
    {
        return controllersHaveFormula( fFormula );
    }

    bool CMathJaxQt6Widget::controllersHaveFormula( const std::optional< QString > &formula ) const
    {
        for ( auto &&ii : fControllingWidgets )
        {
            if ( ii->isFormula( formula ) )
                return true;
        }
        return false;
    }

    void CMathJaxQt6Widget::setFormula( const std::optional< QString > &formula )
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
            showWidget();
            fEngine->renderSVG( fFormula.value() );
        }
    }

    void CMathJaxQt6Widget::setFormulaAndWait( const QString &formula )
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

    int CMathJaxQt6Widget::computePerfectHeight() const
    {
        auto numFormulas = fFormula.has_value() ? this->numFormulas( fFormula.value() ) : 1;
        auto perfectHeight = ( fNumPixelsPerFormula * numFormulas ) - 2 * fScrollBarSize;
        return perfectHeight;
    }

    QSize CMathJaxQt6Widget::idealSVGSize() const
    {
        auto parentWidget = fScrollArea->viewport();
        auto parentSize = parentWidget->size();

        auto scaleToSize = QSize( parentSize.width() - 2 * fScrollBarSize, computePerfectHeight() );
        return svgDefaultSize().scaled( scaleToSize, Qt::AspectRatioMode::KeepAspectRatio );
    }

    QSize CMathJaxQt6Widget::svgDefaultSize() const
    {
        return fSVGWidget->renderer()->defaultSize();
    }

    // if its valid, only show when controllers dont have it
    // if its invalid, show when controllers dont have it or hideInvalid is true

    // HideInvalid | conHave | isValid || visible || hidden
    //=====================================================
    //     0       |    0    |    0    ||  true   || false
    //     0       |    0    |    1    ||  true   || false
    //     0       |    1    |    0    ||  false  || true
    //     0       |    1    |    1    ||  false  || true
    //     1       |    0    |    0    ||  false  || true
    //     1       |    0    |    1    ||  true   || false
    //     1       |    1    |    0    ||  false  || true
    //     1       |    1    |    1    ||  false  || true

    // hidden
    // Hide, conHave     00 | 01 | 11 | 10 |
    // isValid        ----------------------
    //               0|  0  | 1  | 1  | 1  |
    //                ----------------------
    //               1|  0  | 1  | 1  | 0  |

    bool CMathJaxQt6Widget::showWidget( bool ignoreValid /*=false*/ )
    {
        bool controllersHaveFormula = this->controllersHaveFormula();

        bool hide = ignoreValid ? controllersHaveFormula : ( controllersHaveFormula || ( fHideEmptyOrInvalid && !svgValid() ) );
        setHidden( hide );
        if ( !hide )
            emit sigScaleChanged( fScale );
        return !hide;
    }

    void CMathJaxQt6Widget::slotSetNumPixelsPerFormula( int pixelsPerFormula )
    {
        fNumPixelsPerFormula = pixelsPerFormula;
        if ( fAutoSizeToParentWidth || !fAutoUpdateMinimumParentSize || !svgValid() )
        {
            setDefaultMinimumHeight();
            autoScale();
        }
    }

    void CMathJaxQt6Widget::setDefaultMinimumHeight()
    {
        setMinimumSize( 10, fNumPixelsPerFormula + heightPadding() );
    }

    void CMathJaxQt6Widget::slotSetMinScale( double minScale )
    {
        fMinScale = minScale;
        if ( fScale < fMinScale )
            autoScale();
    }

    void CMathJaxQt6Widget::slotSetMaxScale( double maxScale )
    {
        fMaxScale = maxScale;
        if ( fScale > fMaxScale )
            autoScale();
    }

    void CMathJaxQt6Widget::slotSetAutoUpdateMinimumParentSize( bool autoUpdateMinimumParentSize )
    {
        fAutoUpdateMinimumParentSize = autoUpdateMinimumParentSize;
        if ( fAutoSizeToParentWidth )
            autoScale();
    }

    void CMathJaxQt6Widget::slotHideEmptyOrInvalid( bool hideEmptyOrInvalid )
    {
        fHideEmptyOrInvalid = hideEmptyOrInvalid;
        showWidget();
    }

    bool CMathJaxQt6Widget::svgValid() const
    {
        return fSVGWidget->renderer()->isValid();
    }

    void CMathJaxQt6Widget::setupUI()
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
    }

}