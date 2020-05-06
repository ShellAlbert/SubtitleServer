#include "ztimeinoutdrawer.h"
#include <QPainter>
#include <QDebug>
ZTimeInOutDrawer::ZTimeInOutDrawer(QWidget *parent) :
    QWidget(parent)
{
    this->m_TimeIn=0;
    this->m_FadeUpEnd=0;
    this->m_FadeDownStart=0;
    this->m_TimeOut=0;
}

void ZTimeInOutDrawer::ZFunctionDrawTimeInOut(quint32 fadeUpAlphaStep,quint32 fadeDownAlphaStep,quint32 timeIn,quint32 fadeUpEnd,quint32 fadeDownStart,quint32 timeOut,quint32 rate)
{
    this->m_FadeUpAlphaStep=fadeUpAlphaStep;
    this->m_FadeDownAlphaStep=fadeDownAlphaStep;
    this->m_TimeIn=timeIn;
    this->m_FadeUpEnd=fadeUpEnd;
    this->m_FadeDownStart=fadeDownStart;
    this->m_TimeOut=timeOut;
    this->m_rate=rate;
    this->update();
}

void ZTimeInOutDrawer::paintEvent(QPaintEvent *event)
{
    QPainter tPainter(this);
    quint32 tWidth=this->size().width();
    quint32 tHeight=this->size().height();
//    qDebug()<<tWidth<<tHeight;
//    tPainter.fillRect(QRectF(0,0,tWidth,tHeight),QBrush(QColor(28,120,0)));
    if(this->m_TimeIn<=0 && this->m_FadeUpEnd<=0 && this->m_FadeDownStart<=0 && this->m_TimeOut<=0)
    {
        return;
    }
    //resale.
    quint32 tTimeIn=0;
    quint32 tFadeUpEnd=this->m_FadeUpEnd-this->m_TimeIn;
    quint32 tFadeDownStart=this->m_FadeDownStart-this->m_TimeIn;
    quint32 tTimeOut=this->m_TimeOut-this->m_TimeIn;

    qDebug()<<"TimeInOut:"<<tTimeIn<<tFadeUpEnd<<tFadeDownStart<<tTimeOut;

    //draw the scale number.
    quint32 nScale=tWidth/tTimeOut;
    //draw the base line with blue color.
    quint32 tBaseLineRectHeigth=15;
    quint32 tBaseLineRectWidth=tWidth;
    QRect tBaseLineRect(tTimeIn*nScale,tHeight-tBaseLineRectHeigth,tBaseLineRectWidth,tBaseLineRectHeigth);
    tPainter.fillRect(tBaseLineRect,QBrush(QColor(0,0,255)));
    //draw FadeUpEnd scale with red.
    QRect tFadeUpEndRect(tFadeUpEnd*nScale,0,10,tHeight);
    tPainter.fillRect(tFadeUpEndRect,QBrush(QColor(255,0,0)));
    //draw FadeDownStart scale with yellow.
    QRect tFadeDownStartRect(tFadeDownStart*nScale,0,10,tHeight);
    tPainter.fillRect(tFadeDownStartRect,QBrush(QColor(0xf1,0x0a,0xee)));

#if 0
    QString tStr;
    tStr.sprintf("Scale Factor %d%%",nScale);
    int tStrWidth=tPainter.fontMetrics().width(tStr);
    int tStrHeight=tPainter.fontMetrics().height();
    //qDebug()<<"FontSize:"<<tStrWidth<<tStrHeight;
    QPointF  tStrPos((tWidth-tStrWidth)/2,20);
    //qDebug()<<"Pos:"<<tStrPos.x()<<tStrPos.y();
    tPainter.drawText(tStrPos,tStr);
#endif
    //draw circlr.
    /**
     * @brief tPieRect
     * The startAngle and spanAngle must be specified in 1/16th of a degree, i.e. a full circle equals 5760 (16 * 360).
     * Positive values for the angles mean counter-clockwise while negative values mean the clockwise direction.
     * Zero degrees is at the 3 o'clock position.
     */

    QRectF  tPieRect((tWidth-tHeight-5)/2,5,tHeight-5,tHeight-5);
    tPainter.setPen(QPen(Qt::yellow,3,Qt::SolidLine));
    //draw the background circle.
    tPainter.setBrush(QColor(0,255,0));
    tPainter.drawPie(tPieRect,0,360*16);
    //draw the pie.
    tPainter.setBrush(QColor(255,0,0));
    tPainter.drawPie(tPieRect,0,this->m_rate*16);
}
QSize ZTimeInOutDrawer::sizeHint() const
{
    return QSize(100,100);
}
