
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <memory>

class QSvgWidget;
class CMathFormulaEngine;

namespace Ui
{
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow( QWidget *parent = 0 );
    ~MainWindow();

private slots:
    void runMathJax();

private:
    std::unique_ptr< Ui::MainWindow > fImpl;
    QSvgWidget *fSVG{ nullptr };
    CMathFormulaEngine *fEngine{ nullptr };
};

#endif   // MAINWINDOW_H
