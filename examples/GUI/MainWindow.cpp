#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "include/Qt6MathJax.h"

#include <QWebEngineView>
#include <QWebEnginePage>
#include <QSvgRenderer>

#include <QMessageBox>
#include <QTimer>

#include <algorithm>

CMainWindow::CMainWindow( QWidget *parent ) :
    QMainWindow( parent ),
    fImpl( new Ui::CMainWindow )
{
    NTowel42::CQt6MathJax::enableDebugConsole( 12345 );
    QLoggingCategory::setFilterRules( ( QStringList() << "*=false" << "js=true" << "Towel42.Qt6MathJax=true" << "Towel42.Qt6MathJax.*=true" ).join( "\n" ) );
    fEngine = new NTowel42::CQt6MathJax;

    fImpl->setupUi( this );
    fImpl->lineEdit->setText( R"(x = {-b \pm \sqrt{b^2-4ac} \over 2a})" );
    fImpl->webEngineViewLayout->addWidget( fEngine->webEngineViewWidget() );
    connect( fImpl->lineEdit, &QLineEdit::returnPressed, fImpl->asyncRender, &QPushButton::animateClick );
    connect( fImpl->lineEdit, &QLineEdit::textChanged, this, &CMainWindow::slotEnableInput );
    connect(
        fImpl->asyncRender, &QPushButton::clicked,
        [ = ]()
        {
            clear();
            QApplication::setOverrideCursor( Qt::WaitCursor );
            fEngine->renderSVG( fImpl->lineEdit->text() );
        } );
    connect( fImpl->syncRender, &QPushButton::clicked, this, &CMainWindow::slotSyncRender );

    fImpl->lineEdit->setEnabled( false );
    fImpl->asyncRender->setEnabled( false );
    fImpl->syncRender->setEnabled( false );

    connect( fEngine, &NTowel42::CQt6MathJax::sigEngineReady, this, &CMainWindow::slotEngineReady );
    connect( fEngine, &NTowel42::CQt6MathJax::sigErrorMessage, this, &CMainWindow::slotErrorMessage );
    connect( fEngine, &NTowel42::CQt6MathJax::sigSVGRendered, this, &CMainWindow::slotSVGRendered );
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
    fImpl->svgWidget->load( QString() );
}

void CMainWindow::loadSVG( const QByteArray &svg )
{
    fImpl->plainTextEdit->setPlainText( svg );

    fImpl->svgWidget->load( svg );
    if ( !fImpl->svgWidget->renderer()->isValid() )
    {
        slotErrorMessage( tr( "Could not load the SVG file" ) );
        fEngine->clearCache( fImpl->lineEdit->text() );
    }
    else
    {
        auto sz = fImpl->svgWidget->sizeHint().scaled( QSize( 200, 0 ), Qt::KeepAspectRatioByExpanding );
        fImpl->svgWidget->setMinimumSize( sz );
        fImpl->svgWidget->setMaximumSize( sz );
    }
    QApplication::restoreOverrideCursor();
}

void CMainWindow::slotSVGRendered( const QString & /*tex*/, const QByteArray &svg )
{
    loadSVG( svg );
}

void CMainWindow::slotSyncRender()
{
    clear();
    QApplication::setOverrideCursor( Qt::WaitCursor );
    QByteArray svgCode;
    fEngine->renderSVG(
        fImpl->lineEdit->text(),   //
        [ = ]( const std::optional< QByteArray > &svg )   //
        {
            if ( !svg.has_value() )
            {
                QMessageBox::critical( this, tr( "Error in MathJax Engine" ), fEngine->errorMessage() );
                return;
            }

            loadSVG( svg.value() );
        } );
}
