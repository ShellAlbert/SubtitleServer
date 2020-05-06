#ifndef ZNETWORKPLAYER_H
#define ZNETWORKPLAYER_H

#include <QWidget>
#include <QProgressBar>
#include <QPushButton>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QTableWidget>
#include <QQueue>
#include <QImage>
#include <networkPlayer/ztcpserver.h>
#include <networkPlayer/ztimeinoutdrawer.h>
class ZNetworkPlayer : public QWidget
{
    Q_OBJECT
public:
    explicit ZNetworkPlayer(QWidget *parent = 0);
    ~ZNetworkPlayer();
signals:
    void ZSignalClearScreen();
    void ZSignalReceivedNewSubtitleImage(QImage image,quint32 leftTopX,quint32 leftTopY);
public slots:
    void ZSlotStartStopListen();
    void ZSlotAddNewSubtitleToTable(quint32 spotNumber,//
                                    quint32 x,quint32 y,//
                                    quint32 width,quint32 height,//
                                    quint32 compressSize,quint32 decompressSize,//
                                    quint8 fadeUpAlphaStep,quint8 fadeDownAlphaStep,//
                                    quint32 timeIn,quint32 fadeUp,//
                                    quint32 fadeDown,quint32 timeOut);

    void ZSlotSetSubtitleSize(quint32 width,quint32 height);
    void ZSlotSetSubtitleCurrentPixel(quint32 value);
private:
    //buffer status.
    QLabel *m_labelBuffer;
    QProgressBar *m_progressBar;
    QTableWidget *m_tableWidget;
    //control button.
    QLabel *m_labelControl;
    QPushButton *m_btnStartStop;
    QHBoxLayout *m_hlayoutBtn;

    //layout main.
    QVBoxLayout *m_vlayoutMain;

    //TCP server.
    ZTCpServer *m_tcpServer;

    //TimeInOut Drawer.
    ZTimeInOutDrawer *m_timeInOutDrawer;
};

#endif // ZNETWORKPLAYER_H
