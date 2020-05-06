#include "ztcpthread.h"
#include <QDataStream>
#include <zparam.h>
ZTcpThread::ZTcpThread(QObject *parent) :
    QThread(parent)
{
    //allocate a buffer.
    this->m_buffer=new quint8[1*1024*1024];
}
ZTcpThread::~ZTcpThread()
{
    delete this->m_tcpSocket;
    delete []this->m_buffer;
    qDebug()<<"destroy me";
}
void ZTcpThread::ZFunctionSetSocketFd(qint32 sockFd)
{
    this->m_sockFd=sockFd;
}
void ZTcpThread::ZFunctionSetFIFO(QQueue<ZFIFOElement*> *fifo)
{
    this->m_fifo=fifo;
}
void ZTcpThread::ZFunctionSetFIFOMutex(QMutex *mutex,QWaitCondition *fifoNotFull,QWaitCondition *fifoNotEmpty)
{
    this->m_mutex=mutex;
    this->m_condFIFONotFull=fifoNotFull;
    this->m_condFIFONotEmpty=fifoNotEmpty;
}
void ZTcpThread::run()
{
    this->m_bytesTotal=0;
    this->m_bytesReceived=0;
    this->m_tcpSocket=new QTcpSocket;
    this->m_tcpSocket->setSocketDescriptor(this->m_sockFd);
    connect(this->m_tcpSocket,SIGNAL(readyRead()),this,SLOT(ZSlotReadDataFromSocket()),Qt::BlockingQueuedConnection);
    connect(this->m_tcpSocket,SIGNAL(disconnected()),this,SLOT(quit()));
    connect(this->m_tcpSocket,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(ZSlotSocketError(QAbstractSocket::SocketError)));
    this->exec();
}
void ZTcpThread::ZSlotReadDataFromSocket()
{
    //receive the first 32-bit data length.
    if(this->m_bytesReceived<=sizeof(quint32))
    {
        if(this->m_tcpSocket->bytesAvailable()>=sizeof(quint32) && (this->m_bytesTotal==0))
        {
            if(sizeof(quint32)!=this->m_tcpSocket->read((char*)this->m_buffer,sizeof(quint32)))
            {
                qDebug()<<"receive data length failed!";
            }else{
                this->m_bytesTotal=(quint32)(this->m_buffer[0]<<24)|(this->m_buffer[1]<<16)|(this->m_buffer[2]<<8)|(this->m_buffer[3]<<0);
                qDebug()<<"data length:"<<this->m_bytesTotal;
                this->m_bytesReceived+=sizeof(quint32);
            }
        }
    }
    //receive real data section.
    if(this->m_bytesReceived-sizeof(qint32)<this->m_bytesTotal)
    {
        qint64 tNewBytes=this->m_tcpSocket->bytesAvailable();
        qDebug()<<"will read :"<<tNewBytes;
        if(tNewBytes>0)
        {
            //m_bytesReceived hold the data length,4 bytes,
            //so here should subtract 4.
            if(tNewBytes!=this->m_tcpSocket->read((char*)(this->m_buffer+this->m_bytesReceived-sizeof(quint32)),tNewBytes))
            {
                qDebug()<<"read data body error";
            }else{
                this->m_bytesReceived+=tNewBytes;
            }
        }
    }
    //received success.
    if(this->m_bytesReceived-sizeof(qint32)==this->m_bytesTotal)
    {
        qDebug()<<"receive one frame finished!";
        //put data into queue.
        //check count to avoid overflow.
        this->m_mutex->lock();
        while(this->m_fifo->count()>=MAX_FIFO_ELEMENTS)
        {
            this->m_condFIFONotFull->wait(this->m_mutex);
        }
        ZFIFOElement *tElement=new ZFIFOElement;
        tElement->m_totalBytes=this->m_bytesTotal;
        memcpy(tElement->m_buffer,this->m_buffer,this->m_bytesTotal);
        this->m_fifo->enqueue(tElement);
        this->m_condFIFONotEmpty->wakeAll();
        this->m_mutex->unlock();
    }
    return;
}
void ZTcpThread::ZSlotSocketError(QAbstractSocket::SocketError sockError)
{
    switch(sockError)
    {
    case QAbstractSocket::RemoteHostClosedError:
        qDebug()<<"client closed socket";
        break;
    default:
        break;
    }
}
