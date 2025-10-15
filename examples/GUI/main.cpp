#include "MainWindow.h"
#include <QApplication>

#include "include/MathJaxQt6.h"

#include <QLoggingCategory>
#include <QDirIterator>

int main( int argc, char *argv[] )
{
    QApplication appl( argc, argv );
    QLoggingCategory::setFilterRules( ( QStringList() << "*=false" << "js=true" << "Towel42.MathJaxQt6=true" << "Towel42.MathJaxQt6.*=true" ).join( "\n" ) );

    CMainWindow w;
    w.show();

    return appl.exec();
}
