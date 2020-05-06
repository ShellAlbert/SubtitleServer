#ifndef ZTCPSERVER_H
#define ZTCPSERVER_H

#include <QTcpServer>
#include <QQueue>
#include <QMutex>
#include <QWaitCondition>
#include <QImage>
#include <networkPlayer/zfifoelement.h>
#include <networkPlayer/zdrawthread.h>
class ZTCpServer : public QTcpServer
{
    Q_OBJECT
public:
    explicit ZTCpServer(QObject *parent = 0);
    ~ZTCpServer();
protected:
    void incomingConnection(qint32 sockFd);
signals:
    void ZSignalClearScreen();
    void ZSignalReceivedNewImage(QImage image,quint32 leftTopX,quint32 leftTopY);
    void ZSignalReceivedSubtitle(quint32 spotNumber,//
                                 quint32 x,quint32 y,//
                                 quint32 width,quint32 height,//
                                 quint32 compressSize,quint32 decompressSize,//
                                 quint8 fadeUpAlphaStep,quint8 fadeDownAlphaStep,//
                                 quint32 timeIn,quint32 fadeUp,//
                                 quint32 fadeDown,quint32 timeOut);
    void ZSignalSubtitleSize(quint32 width,quint32 height);
    void ZSignalSubtitleCurrentPixel(quint32 value);
public slots:
    void ZSlotAcceptError(QAbstractSocket::SocketError sockError);

public:
    //queue(FIFO).
    QQueue<ZFIFOElement*>  m_fifo;
    QMutex *m_mutex;
    QWaitCondition *m_condFIFONotFull;
    QWaitCondition *m_condFIFONotEmpty;

    //draw thread.
    ZDrawThread *m_drawThread;
};

#endif // ZTCPSERVER_H
