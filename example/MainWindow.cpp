#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "lib/Qt6MathJax.h"

#include <QSvgRenderer>

#include <QXmlStreamReader>
#include <QXmlStreamWriter>

#include <QMessageBox>
#include <QTimer>

#define HT 100

CMainWindow::CMainWindow( QWidget *parent ) :
    QMainWindow( parent ),
    fImpl( new Ui::CMainWindow ),
    fEngine( new CQt6MathJax )
{
    fImpl->setupUi( this );
    fImpl->svgWidget->setMinimumSize( HT, HT );
    fImpl->lineEdit->setText( R"(x = -b \pm \sqrt{b^2-4ac} \over 2a)" );
    connect( fImpl->lineEdit, &QLineEdit::returnPressed, fImpl->render, &QPushButton::animateClick );
    connect(
        fImpl->lineEdit, &QLineEdit::textChanged,   //
        [ this ]()   //
        {   //
            fImpl->lineEdit->setEnabled( fEngine->engineReady() && !fImpl->lineEdit->text().trimmed().isEmpty() );
        }   //
    );
    connect( fImpl->render, &QPushButton::clicked, this, &CMainWindow::generate );

    fImpl->lineEdit->setEnabled( false );

    connect( fEngine, &CQt6MathJax::sigEngineReady, this, &CMainWindow::slotEngineReady );
    connect( fEngine, &CQt6MathJax::sigErrorMessage, this, &CMainWindow::slotErrorMessage );
    connect( fEngine, &CQt6MathJax::sigSVGRendered, this, &CMainWindow::slotSVGRendered );

    QTimer::singleShot(
        0,
        [ = ]()
        {
            QString url = "qrc:/Qt6MathJax/Qt6MathJax.html";
            fImpl->webEngineView->load( url );
        } );
}

CMainWindow::~CMainWindow()
{
}

void CMainWindow::slotEngineReady( bool aOK )
{
    fImpl->lineEdit->setEnabled( aOK );
}

void CMainWindow::slotErrorMessage( const QString &msg )
{
    QMessageBox::critical( this, tr( "Error in MathJax Engine" ), msg );
}

void CMainWindow::slotSVGRendered( const QByteArray &svg )
{
    QString xmlOut;
    QXmlStreamReader reader( svg );
    QXmlStreamWriter writer( &xmlOut );
    writer.setAutoFormatting( true );
    bool invalidElementFound = false;
    while ( !reader.atEnd() )
    {
        reader.readNext();
        if ( reader.tokenType() != QXmlStreamReader::TokenType::Invalid )
        {
            qCInfo( Qt6MathJaxConsole ) << reader.tokenType();
            writer.writeCurrentToken( reader );
        }
        else
        {
            invalidElementFound = true;
        }
    }

    if ( invalidElementFound )
        slotErrorMessage( tr( "Invalid element found in SVG" ) );
    fImpl->plainTextEdit->setPlainText( xmlOut );

    fImpl->svgWidget->load( svg );
    if ( fImpl->svgWidget->renderer()->isValid() )
    {
        auto sz = fImpl->svgWidget->sizeHint();
        sz.setWidth( sz.width() * HT / sz.height() );
        sz.setHeight( HT * 1.2 );
        fImpl->svgWidget->setMinimumSize( sz );
        fImpl->svgWidget->setMaximumSize( sz );
    }
    else
    {
        slotErrorMessage( tr( "Could not load the SVG file" ) );
    }
}

void CMainWindow::generate()
{
    fEngine->renderSVG( fImpl->lineEdit->text() );
}
