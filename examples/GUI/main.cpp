#include "MainWindow.h"
#include <QApplication>

#include "include/Qt6MathJax.h"

#include <QLoggingCategory>
#include <QDirIterator>

int main( int argc, char *argv[] )
{
    QApplication appl( argc, argv );
    QLoggingCategory::setFilterRules( ( QStringList() << "*=false" << "js=true" << "Towel42.Qt6MathJax=true" << "Towel42.Qt6MathJax.*=true" ).join( "\n" ) );

    CMainWindow w;
    w.show();

    return appl.exec();
}
