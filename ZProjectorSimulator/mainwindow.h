#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QStackedWidget>
#include <staticFrame/staticframewindow.h>
#include <networkPlayer/znetworkplayer.h>
#include <screenSimulator/screensimulator.h>
#include <rgbDrawer/rgbdrawer.h>
#include <zbutton/zbutton.h>
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();
private slots:
    void ZSlotShowStaticFrameWindow();
    void ZSlotShowNetworkPlayer();
    void ZSlotShowRGBDrawer();
    void ZSlotShowHideScreen();
protected:
    void closeEvent(QCloseEvent *);
private:
    //trigger buttons.
    ZButton *m_btnStaticFrame;
    ZButton *m_btnNetworkPlayer;
    ZButton *m_btnRGBDrawer;
    ZButton *m_btnShowHideScreen;
    QVBoxLayout *m_vlayoutBtns;
    //container.
    QStackedWidget *m_stackedWidget;
    //sub windows.
    StaticFrameWindow *m_staticFrameWin;
    //network player.
    ZNetworkPlayer *m_networkPlayer;
    //screen simulator.
    ScreenSimulator *m_screenSimu;
    //RGBDrawer.
    RGBDrawer *m_RGBDrawer;

    //layout main
    QWidget *m_mainWidget;
    QHBoxLayout *m_hlayoutMain;
};

#endif // MAINWINDOW_H
