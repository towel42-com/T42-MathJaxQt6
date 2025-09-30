
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <memory>

namespace NTowel42
{
    class CQt6MathJax;
}

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
    void slotEngineReady( bool aOK );
    void slotSVGRendered( const QByteArray &svg );

private:
    void loadSVG( const QByteArray &svg );

    void slotErrorMessage( const QString &msg );
    void slotEnableInput();
    void slotSyncRender();

private:
    void clear();
    std::unique_ptr< Ui::CMainWindow > fImpl{};
    NTowel42::CQt6MathJax *fEngine{ nullptr };
};

#endif   // MAINWINDOW_H
