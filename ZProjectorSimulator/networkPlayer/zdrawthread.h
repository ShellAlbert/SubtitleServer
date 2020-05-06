#ifndef ZDRAWTHREAD_H
#define ZDRAWTHREAD_H

#include <QThread>
#include <QQueue>
#include <QMutex>
#include <QWaitCondition>
#include <networkPlayer/zfifoelement.h>
#include <QImage>
class ZDrawThread : public QThread
{
    Q_OBJECT
public:
    explicit ZDrawThread(QObject *parent = 0);
public:
    void ZFunctionSetFIFO(QQueue<ZFIFOElement*> *fifo);
    void ZFunctionSetFIFOMutex(QMutex *mutex,QWaitCondition *fifoNotFull,QWaitCondition *fifoNotEmpty);
private:
    void ZFunctionHandleFIFOElement(ZFIFOElement *tElement);
protected:
    void run();
signals:
    void ZSignalClearScreen();
    void ZSignalNewSubtitleImage(QImage image,quint32 leftTopX,quint32 leftTopY);
    void ZSignalFetchNewSubtitle(quint32 spotNumber,//
                                 quint32 x,quint32 y,//
                                 quint32 width,quint32 height,//
                                 quint32 compressSize,quint32 decompressSize,//
                                 quint8 fadeUpAlphaStep,quint8 fadeDownAlphaStep,//
                                 quint32 timeIn,quint32 fadeUpEnd,//
                                 quint32 fadeDownStart,quint32 timeOut);
    void ZSignalSubtitleSize(quint32 width,quint32 height);
    void ZSignalSubtitleCurrentPixel(quint32 value);
public slots:

private:
    QQueue<ZFIFOElement*> *m_fifo;
    QMutex *m_mutex;
    QWaitCondition *m_condFIFONotFull;
    QWaitCondition *m_condFIFONotEmpty;
};

#endif // ZDRAWTHREAD_H
