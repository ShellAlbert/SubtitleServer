#ifndef SCREENSIMULATOR_H
#define SCREENSIMULATOR_H

#include <QWidget>
#include <QPaintEvent>
#include <QImage>
#include <QList>
#include <screenSimulator/frameelement.h>
enum{
    ZDraw_Type_Normal=0,//
    ZDraw_Type_ClearScreen=1,//
    ZDraw_Type_NetworkPlayer=2,///<
};
class ScreenSimulator : public QWidget
{
    Q_OBJECT
public:
    explicit ScreenSimulator(QWidget *parent = 0);
    ~ScreenSimulator();
protected:
    QSize sizeHint() const;
    void paintEvent(QPaintEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
signals:

public slots:
    void ZSlotClearScreen();
    void ZSlotSetFileName(const QString fileName,quint8 bDrawOutline);
    void ZSlotDrawRGBStream(quint32 width,quint32 height,QString fileName);

    //for network player.
public slots:
    void ZSlotDrawSubtitleImage(QImage image,quint32 leftTopX,quint32 leftTopY);
private:
    QImage m_networkImage;
    quint32 m_networkImageLeftTopX;
    quint32 m_networkImageLeftTopY;
private:
    bool ZFunctionDeCompress();
private:
    QString m_fileName;
    quint32 tSpotNumber;
    quint32 tLeftTopX,tLeftTopY;
    quint32 tPicWidth,tPicHeight;
    quint32 tTimeIn,tTimeOut;
    quint8 tSubtitleTotalNo,tSubtitleCurrentNo;
    quint32 tFadeUpEnd,tFadeDownStart;
    quint32 tDataLength;
    //for store compressed data.
    quint8 *m_CompressedData;
    quint32 m_DataBodyStart;
    //for store decompress data.
    quint8 *m_PixelData;
    quint32 m_PixelDataLen;


    //screen simulator.
    QImage *mScreenImage;

    //draw type.
    quint32 m_drawType;

    //list to contain all frame elements.
    QList<FrameElement*> m_frameElementList;

    quint8  m_bDrawOutline;


};

#endif // SCREENSIMULATOR_H
