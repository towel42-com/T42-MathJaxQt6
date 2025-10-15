#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "include/MathJaxQt6.h"

#include <QWebEngineView>
#include <QWebEnginePage>

#include <QMessageBox>
#include <QTimer>

#include <algorithm>

CMainWindow::CMainWindow( QWidget *parent ) :
    QMainWindow( parent ),
    fImpl( new Ui::CMainWindow )
{
    NTowel42::CMathJaxQt6::enableDebugConsole( 12345 );
    QLoggingCategory::setFilterRules( ( QStringList() << "*=false" << "js=true" << "Towel42.MathJaxQt6=true" << "Towel42.MathJaxQt6.*=true" ).join( "\n" ) );
    fEngine = new NTowel42::CMathJaxQt6;
    connect(
        fEngine, &NTowel42::CMathJaxQt6::sigSVGRendered,   //
        [ = ]( const QString & /*tex*/, const QByteArray &svg )   //
        {   //
            fImpl->svgCode->setPlainText( svg );
        } );

    fImpl->setupUi( this );

    this->statusBar()->addPermanentWidget( fScaleLabel = new QLabel( this ) );

    fImpl->mathJaxWidget->setEngine( fEngine );

    connect( fImpl->mathJaxWidget, &NTowel42::CMathJaxQt6Widget::sigErrorMessage, this, &CMainWindow::slotErrorMessage );
    connect( fImpl->mathJaxWidget, &NTowel42::CMathJaxQt6Widget::sigScaleChanged, this, &CMainWindow::slotScaleChanged );

    fImpl->lineEdit->setText( R"__(x = {-b \pm \sqrt{b^2-4ac} \over 2a})__" );
    fImpl->webEngineViewLayout->addWidget( fEngine->webEngineViewWidget() );
    connect( fImpl->lineEdit, &QLineEdit::returnPressed, fImpl->asyncRender, &QPushButton::animateClick );
    connect( fImpl->lineEdit, &QLineEdit::textChanged, this, &CMainWindow::slotEnableInput );
    connect(
        fImpl->asyncRender, &QPushButton::clicked,
        [ = ]()
        {
            clear();
            fImpl->mathJaxWidget->setFormula( fImpl->lineEdit->text() );
        } );
    connect( fImpl->syncRender, &QPushButton::clicked, this, &CMainWindow::slotSyncRender );

    fImpl->lineEdit->setEnabled( false );
    fImpl->asyncRender->setEnabled( false );
    fImpl->syncRender->setEnabled( false );

    connect( fEngine, &NTowel42::CMathJaxQt6::sigEngineReady, this, &CMainWindow::slotEngineReady );

    connect( fImpl->autoSizeToParentWidth, &QCheckBox::toggled, fImpl->mathJaxWidget, &NTowel42::CMathJaxQt6Widget::slotSetAutoSizeToParentWidth );
    connect( fImpl->autoUpdateMinimumParentSize, &QCheckBox::toggled, fImpl->mathJaxWidget, &NTowel42::CMathJaxQt6Widget::slotSetAutoUpdateMinimumParentSize );
    connect( fImpl->hideEmptyOrInvalid, &QCheckBox::toggled, fImpl->mathJaxWidget, &NTowel42::CMathJaxQt6Widget::slotHideEmptyOrInvalid );
    connect(
        fImpl->minScale, &QDoubleSpinBox::valueChanged,   //
        [ = ]( double value )
        {
            fImpl->mathJaxWidget->slotSetMinScale( value );
            fImpl->maxScale->setMinimum( value );
            fImpl->scale->setMinimum( value );
        } );
    connect(
        fImpl->maxScale, &QDoubleSpinBox::valueChanged,   //
        [ = ]( double value )
        {
            fImpl->mathJaxWidget->slotSetMaxScale( value );
            fImpl->minScale->setMaximum( value );
            fImpl->scale->setMaximum( value );
        } );
    connect( fImpl->pixelsPerFormula, &QSpinBox::valueChanged, fImpl->mathJaxWidget, &NTowel42::CMathJaxQt6Widget::slotSetNumPixelsPerFormula );
    connect( fImpl->scale, &QDoubleSpinBox::valueChanged, fImpl->mathJaxWidget, &NTowel42::CMathJaxQt6Widget::slotSetScale );

    loadValues();
}

CMainWindow::~CMainWindow()
{
}

void CMainWindow::slotEngineReady( bool /*aOK*/ )
{
    slotEnableInput();
}

void CMainWindow::slotErrorMessage( const QString &msg )
{
    QMessageBox::critical( this, tr( "Error in MathJax Engine" ), msg );
}

void CMainWindow::slotEnableInput()
{
    bool enabled = fEngine->engineReady() && !fImpl->lineEdit->text().trimmed().isEmpty();
    fImpl->lineEdit->setEnabled( enabled );
    fImpl->syncRender->setEnabled( enabled );
    fImpl->asyncRender->setEnabled( enabled );
}

void CMainWindow::clear()
{
    fImpl->svgCode->clear();
    fImpl->mathJaxWidget->clear();
}

void CMainWindow::slotSyncRender()
{
    clear();
    fImpl->mathJaxWidget->setFormulaAndWait( fImpl->lineEdit->text() );
}

void CMainWindow::slotScaleChanged( double scale )
{
    fScaleLabel->setText( QString( "Scale: %1%" ).arg( scale* 100, 0, 'f', 4 ) );
    fImpl->scale->blockSignals( true );
    fImpl->scale->setValue( scale );
    fImpl->scale->blockSignals( false );
}

void CMainWindow::loadValues()
{
    fImpl->autoUpdateMinimumParentSize->blockSignals( true );
    fImpl->hideEmptyOrInvalid->blockSignals( true );
    fImpl->minScale->blockSignals( true );
    fImpl->maxScale->blockSignals( true );
    fImpl->pixelsPerFormula->blockSignals( true );
    fImpl->autoSizeToParentWidth->blockSignals( true );

    fImpl->autoUpdateMinimumParentSize->setChecked( fImpl->mathJaxWidget->autoUpdateMinimumParentSize() );
    fImpl->hideEmptyOrInvalid->setChecked( fImpl->mathJaxWidget->hideEmptyOrInvalid() );
    fImpl->autoSizeToParentWidth->setChecked( fImpl->mathJaxWidget->autoSizeToParentWidth() );

    fImpl->minScale->setValue( fImpl->mathJaxWidget->minScale() );
    fImpl->maxScale->setValue( fImpl->mathJaxWidget->maxScale() );
    fImpl->pixelsPerFormula->setValue( fImpl->mathJaxWidget->numPixelsPerFormula() );
    slotScaleChanged( fImpl->mathJaxWidget->scale() );

    fImpl->autoUpdateMinimumParentSize->blockSignals( false );
    fImpl->hideEmptyOrInvalid->blockSignals( false );
    fImpl->minScale->blockSignals( false );
    fImpl->maxScale->blockSignals( false );
    fImpl->pixelsPerFormula->blockSignals( false );
    fImpl->autoSizeToParentWidth->blockSignals( false );
}