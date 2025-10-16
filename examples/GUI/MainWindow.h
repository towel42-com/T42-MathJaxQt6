
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <memory>

class QLabel;
namespace NTowel42
{
    class CMathJaxQt6;
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
    void slotScaleChanged( double scale );

private:
    void slotErrorMessage( const QString &msg );
    void slotEnableInput();
    void slotSyncRender();

private:
    void loadValues();
    void clear();
    QLabel *fScaleLabel{ nullptr };
    std::unique_ptr< Ui::CMainWindow > fImpl{};
    std::shared_ptr< NTowel42::CMathJaxQt6 > fEngine;
};

#endif   // MAINWINDOW_H
