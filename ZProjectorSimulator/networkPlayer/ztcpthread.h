#ifndef ZTCPTHREAD_H
#define ZTCPTHREAD_H

#include <QThread>
#include <QTcpSocket>
#include <QQueue>
#include <QMutex>
#include <QWaitCondition>
#include <networkPlayer/zfifoelement.h>
class ZTcpThread : public QThread
{
    Q_OBJECT
public:
    explicit ZTcpThread(QObject *parent = 0);
    ~ZTcpThread();
public:
    void ZFunctionSetSocketFd(qint32 sockFd);
    void ZFunctionSetFIFO(QQueue<ZFIFOElement*> *fifo);
    void ZFunctionSetFIFOMutex(QMutex *mutex,QWaitCondition *fifoNotFull,QWaitCondition *fifoNotEmpty);
protected:
    void run();
signals:
    void ZSignalClearScreen();
public slots:
    void ZSlotReadDataFromSocket();
    void ZSlotSocketError(QAbstractSocket::SocketError sockError);
private:
    qint32  m_sockFd;
    QTcpSocket *m_tcpSocket;

    //data buffer.
    quint8 *m_buffer;
    quint32 m_bytesTotal;
    quint32 m_bytesReceived;

    //
    QQueue<ZFIFOElement*> *m_fifo;
    QMutex *m_mutex;
    QWaitCondition *m_condFIFONotFull;
    QWaitCondition *m_condFIFONotEmpty;
};

#endif // ZTCPTHREAD_H
