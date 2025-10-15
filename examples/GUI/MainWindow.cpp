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

    fImpl->setupUi( this );

    fImpl->mathJaxWidget->setEngine( fEngine );
    fImpl->mathJaxWidget->slotSetPixelsPerFormula( fImpl->pixelsPerFormula->value() );
    connect( fImpl->mathJaxWidget, &NTowel42::CMathJaxQt6Widget::sigErrorMessage, this, &CMainWindow::slotErrorMessage );

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
    connect( fImpl->pixelsPerFormula, &QSpinBox::valueChanged, fImpl->mathJaxWidget, &NTowel42::CMathJaxQt6Widget::slotSetPixelsPerFormula );
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
    fImpl->plainTextEdit->clear();
    fImpl->mathJaxWidget->clear();
}

void CMainWindow::slotSyncRender()
{
    clear();
    fImpl->mathJaxWidget->setFormulaAndWait( fImpl->lineEdit->text() );
}
