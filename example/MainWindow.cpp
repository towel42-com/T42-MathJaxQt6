#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "lib/Qt6MathJax.h"

#include <QWebEngineFrame>
#include <QWebEngineView>
#include <QWebEnginePage>

#include <QSvgRenderer>

#include <QMessageBox>
#include <QTimer>

#define MINSIZE 100

CMainWindow::CMainWindow( QWidget *parent ) :
    QMainWindow( parent ),
    fImpl( new Ui::CMainWindow ),
    fEngine( new CQt6MathJax )
{
    fImpl->setupUi( this );
    fImpl->svgWidget->setMinimumSize( MINSIZE, MINSIZE );
    fImpl->lineEdit->setText( R"(x = {-b \pm \sqrt{b^2-4ac} \over 2a})" );
    fImpl->webEngineViewLayout->addWidget( fEngine->webEngineViewWidget() );
    connect( fImpl->lineEdit, &QLineEdit::returnPressed, fImpl->render, &QPushButton::animateClick );
    connect( fImpl->lineEdit, &QLineEdit::textChanged, this, &CMainWindow::slotEnableInput );
    connect( fImpl->render, &QPushButton::clicked, this, &CMainWindow::slotRender );

    fImpl->lineEdit->setEnabled( false );
    fImpl->render->setEnabled( false );

    connect( fEngine, &CQt6MathJax::sigEngineReady, this, &CMainWindow::slotEngineReady );
    connect( fEngine, &CQt6MathJax::sigErrorMessage, this, &CMainWindow::slotErrorMessage );
    connect( fEngine, &CQt6MathJax::sigSVGRendered, this, &CMainWindow::slotSVGRendered );
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
    fImpl->render->setEnabled( enabled );
}

void CMainWindow::slotSVGRendered( const QByteArray &svg )
{
    fImpl->plainTextEdit->setPlainText( svg );

    fImpl->svgWidget->load( svg );
    if ( fImpl->svgWidget->renderer()->isValid() )
    {
        auto sz = fImpl->svgWidget->sizeHint();
        sz.setWidth( sz.width() * MINSIZE / sz.height() );
        sz.setHeight( MINSIZE * 1.2 );
        fImpl->svgWidget->setMinimumSize( sz );
        fImpl->svgWidget->setMaximumSize( sz );
    }
    else
    {
        slotErrorMessage( tr( "Could not load the SVG file" ) );
        fEngine->clearCache( fImpl->lineEdit->text() );
    }
}

void CMainWindow::slotRender()
{
    fEngine->renderSVG( fImpl->lineEdit->text() );
}
