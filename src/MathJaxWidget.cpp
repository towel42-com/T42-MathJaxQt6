#include "include/MathJaxWidget.h"
#include "include/Qt6MathJax.h"
#include "ui_MathJaxWidget.h"

#include <QSvgRenderer>
#include <QRegularExpression>

namespace NTowel42
{
    CMathJaxWidget::CMathJaxWidget( QWidget *parent ) :
        QWidget( parent ),
        fImpl( new Ui::CMathJaxWidget ),
        fEngine( nullptr )
    {
        fImpl->setupUi( this );
        clear();
    }

    CMathJaxWidget::~CMathJaxWidget()
    {
    }

    void CMathJaxWidget::clear()
    {
        fImpl->svgWidget->load( QByteArray() );
        setVisible( false );
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

        fImpl->svgWidget->load( svg );
        if ( !fImpl->svgWidget->renderer()->isValid() )
        {
            emit sigErrorMessage( tr( "Could not load the SVG file" ) );
        }
        else
        {
            setVisible( true );
            updateSVGSize();
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
            QRegularExpressionMatch match = ii.next();

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

    void CMathJaxWidget::updateSVGSize()
    {
        if ( !fFormula.has_value() )
            return;

        if ( !fImpl->svgWidget->isVisible() || !fImpl->svgWidget->renderer()->isValid() )
            return;

        if ( fImpl->svgWidget->renderer()->aspectRatioMode() != Qt::AspectRatioMode::KeepAspectRatio )
            fImpl->svgWidget->renderer()->setAspectRatioMode( Qt::AspectRatioMode::KeepAspectRatio );

        auto maxHeight = fPixelHeightPerFormula * numFormulas( fFormula.value() );

        auto maxSize = QSize( width(), maxHeight );
        auto defaultSize = fImpl->svgWidget->renderer()->defaultSize();

        QSize sz;
        double multiplier = 1.0;
        do
        {
            sz = defaultSize.scaled( maxSize * multiplier, Qt::KeepAspectRatio );
            multiplier += 0.1;
        }
        while ( sz.height() < ( 0.3 * fPixelHeightPerFormula ) );

        auto buffer = std::max( 30, std::min( static_cast< int >( sz.height() * 0.25 ), fPixelHeightPerFormula ) );
        auto maxParentHeight = sz.height() + buffer;
        setMaximumHeight( maxParentHeight );
        setMinimumHeight( sz.height() );
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
        updateSVGSize();
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
}