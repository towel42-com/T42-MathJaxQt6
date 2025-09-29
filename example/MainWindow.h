
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <memory>

class CQt6MathJax;

namespace Ui
{
    class CMainWindow;
}

class CMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit CMainWindow( QWidget *parent = 0 );
    ~CMainWindow();

private slots:
    void generate();
    void slotEngineReady( bool aOK );
    void slotSVGRendered( const QByteArray &svg );
    void slotErrorMessage( const QString &msg );


private:
    std::unique_ptr< Ui::CMainWindow > fImpl{};
    CQt6MathJax *fEngine{ nullptr };
};

#endif   // MAINWINDOW_H
