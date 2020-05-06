#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    this->setWindowTitle(tr("ZProjectorSimulator-Controller"));
    //create left control buttons.
    this->m_btnStaticFrame=new ZButton;
    this->m_btnStaticFrame->setIcon(QIcon(":/images/48x48/subtitle.png"));
    this->m_btnStaticFrame->setToolTip(tr("Single y Frame Simulation"));
    connect(this->m_btnStaticFrame,SIGNAL(clicked()),this,SLOT(ZSlotShowStaticFrameWindow()));

    //real time simulate.
    this->m_btnNetworkPlayer=new ZButton;
    this->m_btnNetworkPlayer->setIcon(QIcon(":/images/48x48/player.png"));
    this->m_btnNetworkPlayer->setToolTip(tr("Realtime Network Subtitle Simulation"));
    connect(this->m_btnNetworkPlayer,SIGNAL(clicked()),this,SLOT(ZSlotShowNetworkPlayer()));


    //draw RGB stream.
    this->m_btnRGBDrawer=new ZButton;
    this->m_btnRGBDrawer->setIcon(QIcon(":/images/48x48/rgb.png"));
    this->m_btnRGBDrawer->setToolTip(tr("draw RGB code stream"));
    connect(this->m_btnRGBDrawer,SIGNAL(clicked()),this,SLOT(ZSlotShowRGBDrawer()));

    //show or hide screen.
    this->m_btnShowHideScreen=new ZButton;
    this->m_btnShowHideScreen->setIcon(QIcon(":/images/48x48/screen.png"));
    this->m_btnShowHideScreen->setToolTip(tr("Show or Hide Screen"));
    connect(this->m_btnShowHideScreen,SIGNAL(clicked()),this,SLOT(ZSlotShowHideScreen()));


    //layout buttons.
    this->m_vlayoutBtns=new QVBoxLayout;
    this->m_vlayoutBtns->addWidget(this->m_btnStaticFrame);
    this->m_vlayoutBtns->addWidget(this->m_btnNetworkPlayer);
    this->m_vlayoutBtns->addWidget(this->m_btnRGBDrawer);
    this->m_vlayoutBtns->addStretch(1);
    this->m_vlayoutBtns->addWidget(this->m_btnShowHideScreen);

    //container.
    this->m_stackedWidget=new QStackedWidget;
    this->m_stackedWidget->setStyleSheet("QStackedWidget{border:2px solid black;}");

    //sub window.
    this->m_staticFrameWin=new StaticFrameWindow;
    this->m_RGBDrawer=new RGBDrawer;
    this->m_networkPlayer=new ZNetworkPlayer;
    this->m_stackedWidget->addWidget(this->m_staticFrameWin);
    this->m_stackedWidget->addWidget(this->m_RGBDrawer);
    this->m_stackedWidget->addWidget(this->m_networkPlayer);

    //layout in main
    this->m_mainWidget=new QWidget;
    this->setCentralWidget(this->m_mainWidget);
    this->m_hlayoutMain=new QHBoxLayout;
    this->m_hlayoutMain->addLayout(this->m_vlayoutBtns);
    this->m_hlayoutMain->addWidget(this->m_stackedWidget);
    this->m_mainWidget->setLayout(this->m_hlayoutMain);

    //create standalone window for screen simulator.
    this->m_screenSimu=new ScreenSimulator;
    this->m_screenSimu->hide();

    connect(this->m_staticFrameWin,SIGNAL(ZSignalClearScreen()),//
            this->m_screenSimu,SLOT(ZSlotClearScreen()));

    connect(this->m_staticFrameWin,SIGNAL(ZSignalSubtitleFileName(QString,quint8)),//
            this->m_screenSimu,SLOT(ZSlotSetFileName(QString,quint8)));

    connect(this->m_RGBDrawer,SIGNAL(ZSignalRGBDrawer(quint32,quint32,QString)),//
            this->m_screenSimu,SLOT(ZSlotDrawRGBStream(quint32,quint32,QString)));

    //network player.
    connect(this->m_networkPlayer,SIGNAL(ZSignalClearScreen()),//
            this->m_screenSimu,SLOT(ZSlotClearScreen()));
    connect(this->m_networkPlayer,SIGNAL(ZSignalReceivedNewSubtitleImage(QImage,quint32,quint32)),//
            this->m_screenSimu,SLOT(ZSlotDrawSubtitleImage(QImage,quint32,quint32)));
}

MainWindow::~MainWindow()
{
    delete this->m_btnStaticFrame;
    delete this->m_btnNetworkPlayer;
    delete this->m_btnRGBDrawer;
    delete this->m_btnShowHideScreen;
    delete this->m_vlayoutBtns;
    delete this->m_staticFrameWin;
    delete this->m_RGBDrawer;
    delete this->m_networkPlayer;
    delete this->m_stackedWidget;
    delete this->m_hlayoutMain;
    delete this->m_screenSimu;
}
void MainWindow::ZSlotShowStaticFrameWindow()
{
    this->m_stackedWidget->setCurrentWidget(this->m_staticFrameWin);
}
void MainWindow::ZSlotShowNetworkPlayer()
{
    this->m_stackedWidget->setCurrentWidget(this->m_networkPlayer);
}
void MainWindow::ZSlotShowRGBDrawer()
{
  this->m_stackedWidget->setCurrentWidget(this->m_RGBDrawer);
}

void MainWindow::ZSlotShowHideScreen()
{
    if(this->m_screenSimu->isVisible())
    {
        this->m_screenSimu->hide();
    }else{
        this->m_screenSimu->show();
    }
}
void MainWindow::closeEvent(QCloseEvent *)
{
    this->m_screenSimu->close();
}
