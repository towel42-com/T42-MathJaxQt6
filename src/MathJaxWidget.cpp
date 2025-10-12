#include "include/MathJaxWidget.h"
#include "include/Qt6MathJax.h"
#include "ui_MathJaxWidget.h"

#include <QSvgRenderer>

namespace NTowel42
{
    std::unordered_map< QString, QByteArray > CMathJaxWidget::sFormulaToSVGMap;

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
        fImpl->svgWidget->load( QString() );
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

    void CMathJaxWidget::updateSVGSize()
    {
        if ( !fFormula.has_value() )
            return;

        NTowel42::updateSVGSize( fImpl->svgWidget, fFormula.value(), width() * 0.9, true, fPixelsPerFormula );
    }

    void CMathJaxWidget::slotSVGRendered( const QString &formula, const QByteArray &svg )
    {
        if ( !isFormula( formula ) )
            return;
        loadSVG( svg );
    }

    void CMathJaxWidget::slotSetPixelsPerFormula( int pixelsPerFormula )
    {
        fPixelsPerFormula = pixelsPerFormula;
        updateSVGSize();
    }

    void CMathJaxWidget::setEngine( NTowel42::CQt6MathJax *engine )
    {
        fEngine = engine;
        connect( fEngine, &NTowel42::CQt6MathJax::sigSVGRendered, this, &CMathJaxWidget::slotSVGRendered );
        connect( fEngine, &NTowel42::CQt6MathJax::sigErrorMessage, this, &CMathJaxWidget::sigErrorMessage );
    }

    std::optional< QByteArray > CMathJaxWidget::svgForFormula( const QString &formula ) const
    {
        auto pos = sFormulaToSVGMap.find( formula );
        if ( pos != sFormulaToSVGMap.end() )
        {
            return ( *pos ).second;
        };
        return {};
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
            auto svg = svgForFormula( formula.value() );
            if ( svg.has_value() )
                loadSVG( svg.value() );
            else
            {
                setVisible( false );
                fEngine->renderSVG( fFormula.value() );
            }
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
            [ = ]( const std::optional< QByteArray > &svg )   //
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