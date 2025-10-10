#include "include/MathJaxWidget.h"
#include "include/Qt6MathJax.h"
#include "ui_MathJaxWidget.h"

#include <QSvgRenderer>

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
        fImpl->svgWidget->load( QString() );
        fImpl->frame->setVisible( false );
    }

    void CMathJaxWidget::loadSVG( const QByteArray &svg )
    {
        fImpl->svgWidget->load( svg );
        if ( !fImpl->svgWidget->renderer()->isValid() )
        {
            emit sigErrorMessage( tr( "Could not load the SVG file" ) );
        }
        else
        {
            fImpl->frame->setVisible( true );
            updateSVGSize();
        }
    }

    void CMathJaxWidget::updateSVGSize()
    {
        NTowel42::updateSVGSize( fImpl->svgWidget, fFormula, width() * 0.9, true, fPixelsPerFormula );
    }

    void CMathJaxWidget::slotSVGRendered( const QString &tex, const QByteArray &svg )
    {
        if ( tex != fFormula )
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

    void CMathJaxWidget::setFormula( const QString &formula )
    {
        fFormula = formula;
        fEngine->renderSVG( formula );
    }

    void CMathJaxWidget::setFormulaAndWait( const QString &formula )
    {
        fFormula = formula;
        QApplication::setOverrideCursor( Qt::WaitCursor );
        QByteArray svgCode;
        fEngine->renderSVG(
            fFormula,   //
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