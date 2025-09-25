
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "MathFormulaEngine/MathFormulaEngine.h"
#include <QSvgWidget>

#define HT 100

MainWindow::MainWindow( QWidget *parent ) :
    QMainWindow( parent ),
    fImpl( new Ui::MainWindow ),
    fEngine( new CMathFormulaEngine )
{
    fImpl->setupUi( this );
    //    fImpl->centralWidget->layout()->addWidget( engine.webView() );
    fSVG = new QSvgWidget( fImpl->widget );
    fSVG->setMinimumSize( 0, HT );
    fImpl->widget->setLayout( new QHBoxLayout( fImpl->widget ) );
    fImpl->widget->layout()->addWidget( fSVG );
    connect( fImpl->lineEdit, SIGNAL( returnPressed() ), this, SLOT( runMathJax() ) );
}

MainWindow::~MainWindow()
{
}

void MainWindow::runMathJax()
{
    QString svgCode = fEngine->svg( fImpl->lineEdit->text() );
    if ( fEngine->error().isEmpty() )
    {
        fSVG->load( svgCode.toUtf8() );
        fImpl->plainTextEdit->setPlainText( svgCode );
    }
    else
    {
        //        fSVG->load( QString( "<fSVG></fSVG>" ).toUtf8() );
        fImpl->plainTextEdit->setPlainText( fEngine->error() );
    }
    QSize s = fSVG->sizeHint();
    fImpl->widget->setMinimumSize( QSize( s.width() * HT / s.height(), HT * 1.2 ) );
    fImpl->widget->setMaximumSize( QSize( s.width() * HT / s.height(), HT * 1.2 ) );
    fImpl->widget->layout()->update();
}
