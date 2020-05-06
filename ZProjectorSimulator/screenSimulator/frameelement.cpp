#include "frameelement.h"

FrameElement::FrameElement(QWidget *parent) :
    QWidget(parent)
{
    this->setWindowFlags(Qt::ToolTip);
    //spot number.
    this->m_labelSpotNumber[0]=new QLabel;
    this->m_labelSpotNumber[0]->setText(tr("SpotNumber"));
    this->m_labelSpotNumber[1]=new QLabel;
    //left top x y.
    this->m_labelLeftTopXY[0]=new QLabel;
    this->m_labelLeftTopXY[0]->setText(tr("LeftTopXY"));
    this->m_labelLeftTopXY[1]=new QLabel;
    //size,width*height.
    this->m_labelSize[0]=new QLabel;
    this->m_labelSize[0]->setText(tr("Size"));
    this->m_labelSize[1]=new QLabel;
    //subtitle frames number.
    this->m_labelSubtitleFramesNumber[0]=new QLabel;
    this->m_labelSubtitleFramesNumber[0]->setText(tr("No."));
    this->m_labelSubtitleFramesNumber[1]=new QLabel;
    //layout
    this->m_gridlayoutMain=new QGridLayout;
    this->m_gridlayoutMain->addWidget(this->m_labelSpotNumber[0],0,0);
    this->m_gridlayoutMain->addWidget(this->m_labelSpotNumber[1],0,1);
    this->m_gridlayoutMain->addWidget(this->m_labelLeftTopXY[0],1,0);
    this->m_gridlayoutMain->addWidget(this->m_labelLeftTopXY[1],1,1);
    this->m_gridlayoutMain->addWidget(this->m_labelSize[0],2,0);
    this->m_gridlayoutMain->addWidget(this->m_labelSize[1],2,1);
    this->m_gridlayoutMain->addWidget(this->m_labelSubtitleFramesNumber[0],3,0);
    this->m_gridlayoutMain->addWidget(this->m_labelSubtitleFramesNumber[1],3,1);

    this->setLayout(this->m_gridlayoutMain);
}
FrameElement::~FrameElement()
{
    delete this->m_labelSpotNumber[0];
    delete this->m_labelSpotNumber[1];
    delete this->m_labelLeftTopXY[0];
    delete this->m_labelLeftTopXY[1];
    delete this->m_labelSize[0];
    delete this->m_labelSize[1];
    delete this->m_gridlayoutMain;
}
void FrameElement::ZFunctionSetSpotNumber(quint32 spotNumber)
{
    //hold.
    this->z_spotNumber=spotNumber;
    //display.
    QString spotNumberStr;
    spotNumberStr.sprintf("%d",spotNumber);
    this->m_labelSpotNumber[1]->setText(spotNumberStr);
}
quint32 FrameElement::ZFunctionGetSpotNumber()
{
    return this->z_spotNumber;
}

void FrameElement::ZFunctionSetLeftTopXY(quint32 x,quint32 y)
{
    //hold.
    this->z_x=x;
    this->z_y=y;
    //display.
    QString xyStr;
    xyStr.sprintf("(%d,%d)",x,y);
    this->m_labelLeftTopXY[1]->setText(xyStr);
}
void FrameElement::ZFunctionGetLeftTopXY(quint32 *x,quint32 *y)
{
    *x=this->z_x;
    *y=this->z_y;
}

//width*height.
void FrameElement::ZFunctionSetSize(quint32 width,quint32 height)
{
    //hold.
    this->z_width=width;
    this->z_height=height;
    //display.
    QString str;
    str.sprintf("%d*%d",width,height);
    this->m_labelSize[1]->setText(str);
}
void FrameElement::ZFunctionGetSize(quint32 *width,quint32 *height)
{
    *width=this->z_width;
    *height=this->z_height;
}
//subtitle frames number.
void FrameElement::ZFunctionSetSubtitleFramesNumber(quint32 total,quint32 current)
{
    //hold.
    this->z_framesTotal=total;
    this->z_frameCurrent=current;
    //display.
    QString str;
    str.sprintf("%d-%d",total,current);
    this->m_labelSubtitleFramesNumber[1]->setText(str);
}
