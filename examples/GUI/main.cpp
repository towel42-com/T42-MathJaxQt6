#include "MainWindow.h"
#include <QApplication>

#include "include/Qt6MathJax.h"

#include <QLoggingCategory>
#include <QDirIterator>
#include <QWebEngineView>

void dumpQRC()
{
    if ( !Qt6MathJaxQRC().isDebugEnabled() )
        return;

    QDirIterator ii( ":", QDirIterator::Subdirectories );
    while ( ii.hasNext() )
    {
        // Advance the iterator to the next entry and print its path.
        auto fileName = ii.next();
        auto size = QFileInfo( fileName ).size();
        qCDebug( Qt6MathJaxQRC ).noquote().nospace() << ii.next() << " " << size;
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
