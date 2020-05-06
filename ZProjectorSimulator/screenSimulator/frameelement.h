#ifndef FRAMEELEMENT_H
#define FRAMEELEMENT_H

#include <QWidget>
#include <QGridLayout>
#include <QLabel>
class FrameElement : public QWidget
{
    Q_OBJECT
public:
    explicit FrameElement(QWidget *parent = 0);
    ~FrameElement();

    //spot number.
    void ZFunctionSetSpotNumber(quint32 spotNumber);
    quint32 ZFunctionGetSpotNumber();
    //left top (x,y).
    void ZFunctionSetLeftTopXY(quint32 x,quint32 y);
    void ZFunctionGetLeftTopXY(quint32 *x,quint32 *y);
    //width*height.
    void ZFunctionSetSize(quint32 width,quint32 height);
    void ZFunctionGetSize(quint32 *width,quint32 *height);
    //subtitle frames number.
    void ZFunctionSetSubtitleFramesNumber(quint32 total,quint32 current);
signals:

public slots:

private:
    //spot number.
    QLabel *m_labelSpotNumber[2];
    //left top x & y.
    QLabel *m_labelLeftTopXY[2];
    //size,width*height.
    QLabel *m_labelSize[2];
    //subtitle frames number.
    QLabel *m_labelSubtitleFramesNumber[2];
    QGridLayout *m_gridlayoutMain;


    //interval hold variables.
    quint32 z_spotNumber;
    quint32 z_x;
    quint32 z_y;
    quint32 z_width;
    quint32 z_height;
    quint32 z_framesTotal;
    quint32 z_frameCurrent;
};

#endif // FRAMEELEMENT_H
