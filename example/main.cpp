#include "mainwindow.h"
#include <QApplication>

int main( int argc, char *argv[] )
{
    Q_INIT_RESOURCE( MathFormulaEngine );

    QApplication a( argc, argv );
    MainWindow w;
    w.show();

    //    qDebug() << QDir( ":/" ).entryList();
    //    qDebug() << QDir( ":/MathJax/" ).entryList();

    return a.exec();
}
