#include "ztcpserver.h"
#include <networkPlayer/ztcpthread.h>
ZTCpServer::ZTCpServer(QObject *parent) :
    QTcpServer(parent)
{
    this->m_mutex=new QMutex;
    this->m_condFIFONotFull=new QWaitCondition;
    this->m_condFIFONotEmpty=new QWaitCondition;
    connect(this,SIGNAL(acceptError(QAbstractSocket::SocketError)),this,SLOT(ZSlotAcceptError(QAbstractSocket::SocketError)));

    //create draw thread to handle FIFO's elements.
    this->m_drawThread=new ZDrawThread;
    this->m_drawThread->ZFunctionSetFIFO(&this->m_fifo);
    this->m_drawThread->ZFunctionSetFIFOMutex(this->m_mutex,this->m_condFIFONotFull,this->m_condFIFONotEmpty);
    connect(this->m_drawThread,SIGNAL(ZSignalClearScreen()),this,SIGNAL(ZSignalClearScreen()));
    connect(this->m_drawThread,SIGNAL(ZSignalNewSubtitleImage(QImage,quint32,quint32)),this,SIGNAL(ZSignalReceivedNewImage(QImage,quint32,quint32)));
    connect(this->m_drawThread,SIGNAL(ZSignalFetchNewSubtitle(quint32,quint32,quint32,quint32,quint32,quint32,quint32,quint8,quint8,quint32,quint32,quint32,quint32)),//
            this,SIGNAL(ZSignalReceivedSubtitle(quint32,quint32,quint32,quint32,quint32,quint32,quint32,quint8,quint8,quint32,quint32,quint32,quint32)));
    //update progress bar.
    connect(this->m_drawThread,SIGNAL(ZSignalSubtitleSize(quint32,quint32)),this,SIGNAL(ZSignalSubtitleSize(quint32,quint32)));
    connect(this->m_drawThread,SIGNAL(ZSignalSubtitleCurrentPixel(quint32)),this,SIGNAL(ZSignalSubtitleCurrentPixel(quint32)));

    this->m_drawThread->start();
}
ZTCpServer::~ZTCpServer()
{
    delete this->m_drawThread;
    delete this->m_condFIFONotEmpty;
    delete this->m_condFIFONotFull;
    delete this->m_mutex;
    while(this->m_fifo.count()>0)
    {
        ZFIFOElement *p=this->m_fifo.dequeue();
        delete p;
    }
}
void ZTCpServer::incomingConnection(qint32 sockFd)
{
    qDebug()<<"new connection:"<<sockFd;
    ZTcpThread *tThread=new ZTcpThread;
    tThread->ZFunctionSetSocketFd(sockFd);
    tThread->ZFunctionSetFIFO(&this->m_fifo);
    tThread->ZFunctionSetFIFOMutex(this->m_mutex,this->m_condFIFONotFull,this->m_condFIFONotEmpty);
    connect(tThread,SIGNAL(finished()),tThread,SLOT(deleteLater()));
    tThread->start();
}

void ZTCpServer::ZSlotAcceptError(QAbstractSocket::SocketError sockError)
{
    qDebug()<<"accept error!";
}
