#include "MainWindow.h"
#include <QApplication>

#include "T42-Qt6MathJax/Qt6MathJax.h"

#include <QLoggingCategory>
#include <QDirIterator>

void dumpQRC()
{
    QDirIterator it( ":", QDirIterator::Subdirectories );
    while ( it.hasNext() )
    {
        // Advance the iterator to the next entry and print its path.
        qCInfo( Qt6MathJaxQRC ).noquote().nospace() << it.next();
    }
}

int main( int argc, char *argv[] )
{
    Q_INIT_RESOURCE( Qt6MathJax );

    QApplication a( argc, argv );
    QLoggingCategory::setFilterRules( ( QStringList() << "*=false" << "js=true" << "Qt6MathJax=true" << "Qt6MathJax.Console=true" << "Qt6MathJax.QRC=true" ).join( "\n" ) );

    dumpQRC();

    CMainWindow w;
    w.show();

    return a.exec();
}
