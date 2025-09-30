
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

public:
    ~CMainWindow();

private slots:
    void slotRender();
    void slotEngineReady( bool aOK );
    void slotSVGRendered( const QByteArray &svg );
    void slotErrorMessage( const QString &msg );
    void slotEnableInput();

private:
    std::unique_ptr< Ui::CMainWindow > fImpl{};
    CQt6MathJax *fEngine{ nullptr };
};

#endif   // MAINWINDOW_H
