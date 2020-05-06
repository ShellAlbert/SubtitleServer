#include "zdrawthread.h"
#include <QDebug>
ZDrawThread::ZDrawThread(QObject *parent) :
    QThread(parent)
{
}

void ZDrawThread::ZFunctionSetFIFO(QQueue<ZFIFOElement*> *fifo)
{
    this->m_fifo=fifo;
}
void ZDrawThread::ZFunctionSetFIFOMutex(QMutex *mutex,QWaitCondition *fifoNotFull,QWaitCondition *fifoNotEmpty)
{
    this->m_mutex=mutex;
    this->m_condFIFONotFull=fifoNotFull;
    this->m_condFIFONotEmpty=fifoNotEmpty;
}

void ZDrawThread::run()
{
    qDebug()<<"DrawThread start";
    while(1)
    {
        //wait until fifo not empty.
        this->m_mutex->lock();
        while(this->m_fifo->count()<=0)
        {
            qDebug()<<"DrawThread:wait for fifo has element";
            this->m_condFIFONotEmpty->wait(this->m_mutex);
        }
        ZFIFOElement *tElement=this->m_fifo->dequeue();
        this->ZFunctionHandleFIFOElement(tElement);
        delete tElement;
        this->m_condFIFONotFull->wakeAll();
        this->m_mutex->unlock();
    }
}

void ZDrawThread::ZFunctionHandleFIFOElement(ZFIFOElement *tElement)
{
    qDebug()<<"fetch a element:"<<tElement->m_totalBytes<<" bytes!";
    tElement->ZFunctionDeCompress();
    emit this->ZSignalSubtitleSize(tElement->m_PicWidth,tElement->m_PicHeight);
    QImage image(tElement->m_PicWidth,tElement->m_PicHeight,QImage::Format_ARGB32);
    quint32 tValue=0;
    for(quint32 i=0;i<tElement->m_PicHeight;i++)
    {
        for( quint32 j=0;j<tElement->m_PicWidth;j++)
        {
            quint8  alpha=tElement->m_bufferDecompress[i*tElement->m_PicWidth*sizeof(qint32)+j*sizeof(qint32)+0];
            quint8  red=tElement->m_bufferDecompress[i*tElement->m_PicWidth*sizeof(qint32)+j*sizeof(qint32)+1];
            quint8  green=tElement->m_bufferDecompress[i*tElement->m_PicWidth*sizeof(qint32)+j*sizeof(qint32)+2];
            quint8  blue=tElement->m_bufferDecompress[i*tElement->m_PicWidth*sizeof(qint32)+j*sizeof(qint32)+3];
            image.setPixel(j,i,qRgba(red,green,blue,alpha));
            //emit current pixel.
            emit this->ZSignalSubtitleCurrentPixel(++tValue);
        }
    }
    emit this->ZSignalFetchNewSubtitle(tElement->m_SpotNumber,//
                                       tElement->m_LeftTopX,tElement->m_LeftTopY,//
                                       tElement->m_PicWidth,tElement->m_PicHeight,//<
                                       tElement->m_totalBytes,tElement->m_totalByteDecompress,//
                                       tElement->m_FadeUpAlphaStep,tElement->m_FadeDownAlphaStep,//
                                       tElement->m_TimeIn,tElement->m_FadeUpEnd,//
                                       tElement->m_FadeDownStart,tElement->m_TimeOut);
    emit this->ZSignalNewSubtitleImage(image,tElement->m_LeftTopX,tElement->m_LeftTopY);
    qDebug()<<"draw image finished!";
}
