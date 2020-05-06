#include "screensimulator.h"
#include <QDebug>
#include <QPainter>
#include <QBrush>
ScreenSimulator::ScreenSimulator(QWidget *parent) :
    QWidget(parent)
{
    this->setWindowTitle(tr("ScreenSimulator"));
    this->setWindowFlags(Qt::CustomizeWindowHint|Qt::WindowMinimizeButtonHint);
    this->setMouseTracking(true);
    this->setStyleSheet("QWidget{background-color:rgb(0,0,0);}");
    this->m_CompressedData=new quint8[2*1024*1024];
    this->m_DataBodyStart=0;
    this->m_PixelData=new quint8[2*1024*1024];
    this->m_PixelDataLen=0;

    //we draw the pixel on the image,
    //then scale the image adapted to the widget size.
    //draw the image to the widget.
    this->mScreenImage=new QImage(1024,768,QImage::Format_ARGB32);
    this->setFixedSize(1024,768);
    this->m_drawType=ZDraw_Type_Normal;

    //list to contain all the frame elements.
    this->m_frameElementList.clear();
    this->m_bDrawOutline=0;
}
ScreenSimulator::~ScreenSimulator()
{
    delete [] this->m_CompressedData;
    delete [] this->m_PixelData;
    delete this->mScreenImage;
    this->m_frameElementList.clear();
}
QSize ScreenSimulator::sizeHint() const
{
    return QSize(200,200);
}
bool ScreenSimulator::ZFunctionDeCompress()
{
    quint8 marker;
    quint32 i, j;
    quint32 len_decompress = 0;
    quint32 repeat_times = 0;

    this->m_PixelDataLen=0;
    memset(this->m_PixelData,0,2*1024*1024);
    //check validation.
    if(this->m_DataBodyStart<=0)
    {
        return false;
    }

    quint8 *pSource=&this->m_CompressedData[m_DataBodyStart];
    quint32 sourceLength=this->tDataLength;
    quint8 *pDest=this->m_PixelData;

    //src_data[0]:common marker.
    marker = pSource[0];
    qDebug("Marker=0x%02x\n",marker);

    for (i = 1; i < sourceLength;)
    {
        if (pSource[i] == marker)
        {
            //compressed data.
            //repeat_times = pSource[i + 1];//old code zhangshaoyan@20150116
            repeat_times = pSource[i + 1]+1;//new code.
            for (j = 0; j < repeat_times; j++)
            {
                pDest[len_decompress++] = pSource[i + 2];
                pDest[len_decompress++] = pSource[i + 3];
                pDest[len_decompress++] = pSource[i + 4];
                pDest[len_decompress++] = pSource[i + 5];
            }

            //move to next section.
            //one section format is:
            //1 byte common marker.
            //1 byte repeat times(0xFF maximum).
            // 4 byte data.
            //so here is 6.
            i += sizeof(qint8) * 6;
        }
        else
        {
            //not compressed data.
            // copy to dest directly.
            //move length pointer.
            pDest[len_decompress++] = pSource[i+0];
            pDest[len_decompress++] = pSource[i + 1];
            pDest[len_decompress++] = pSource[i + 2];
            pDest[len_decompress++] = pSource[i + 3];

            //move to next section.
            //bypass this uncompressed section.
            i += sizeof(qint8) * 4;
        }
    }

    this->m_PixelDataLen=len_decompress;
    qDebug()<<"len decompress:"<<len_decompress;
#if 0
    for(qint32 k=0;k<m_PixelDataLen;k+=16)
    {
        qDebug("%02x %02x %02x %02x,%02x %02x %02x %02x,%02x %02x %02x %02x,%02x %02x %02x %02x\n",
               this->m_PixelData[k+0],
                this->m_PixelData[k+1],
                this->m_PixelData[k+2],
                this->m_PixelData[k+3],
                this->m_PixelData[k+4],
                this->m_PixelData[k+5],
                this->m_PixelData[k+6],
                this->m_PixelData[k+7],
                this->m_PixelData[k+8],
                this->m_PixelData[k+9],
                this->m_PixelData[k+10],
                this->m_PixelData[k+11],
                this->m_PixelData[k+12],
                this->m_PixelData[k+13],
                this->m_PixelData[k+14],
                this->m_PixelData[k+15]);
    }
#endif
    return true;
}
void ScreenSimulator::ZSlotClearScreen()
{
    //clear image.
    QPainter tPter(this->mScreenImage);
    QRectF tRect(0,0,this->mScreenImage->size().width(),this->mScreenImage->height());
    QBrush tBrush(QColor(0,0,0));
    tPter.fillRect(tRect,tBrush);
    this->m_drawType=ZDraw_Type_ClearScreen;
    this->update();

    //clear list.
    for(qint32 i=0;i<this->m_frameElementList.count();i++)
    {
        delete this->m_frameElementList.at(i);
    }
    this->m_frameElementList.clear();
}
void ScreenSimulator::ZSlotSetFileName(const QString fileName,quint8 bDrawOutline)
{
    this->m_fileName=fileName;
    if(this->m_fileName.isEmpty())
    {
        return;
    }

    this->m_bDrawOutline=bDrawOutline;

    //open file.
    QFile tFile(this->m_fileName);
    if(tFile.open(QIODevice::ReadOnly)==false)
    {
        qDebug()<<"open file failed!";
        return;
    }
    quint64 tFileLen=tFile.size();
    if(tFileLen<=0)
    {
        return;
    }
    //read data to buffer.
    quint32 tOffset=0;
    if(tFileLen!=tFile.read((char*)this->m_CompressedData,tFileLen))
    {
        qDebug()<<"read failed!";
        return;
    }
    tFile.close();

    //start.
    tOffset=0;
    //bypass 128-bit sync header.
    if(this->m_CompressedData[0]!=0xCC&&this->m_CompressedData[1]!=0xCC &&this->m_CompressedData[2]!=0xCC)
    {
        return;
    }
    tOffset+=sizeof(quint8)*16;

    //SpotNumber,2 bytes.
    tSpotNumber=(quint32)this->m_CompressedData[tOffset+0]<<8|this->m_CompressedData[tOffset+1]<<0;
    tOffset+=sizeof(quint8)*2;
    //LeftTopX,2 bytes.
    tLeftTopX=(quint32)this->m_CompressedData[tOffset+0]<<8|this->m_CompressedData[tOffset+1]<<0;
    tOffset+=sizeof(quint8)*2;

    //LeftTopY,2 bytes.
    tLeftTopY=(quint32)this->m_CompressedData[tOffset+0]<<8|this->m_CompressedData[tOffset+1]<<0;
    tOffset+=sizeof(quint8)*2;

    //picture width,2 bytes.
    tPicWidth=(quint32)this->m_CompressedData[tOffset+0]<<8|this->m_CompressedData[tOffset+1]<<0;
    tOffset+=sizeof(quint8)*2;
    //picture height,2 bytes.
    tPicHeight=(quint32)this->m_CompressedData[tOffset+0]<<8|this->m_CompressedData[tOffset+1]<<0;
    tOffset+=sizeof(quint8)*2;
    //timein,3 bytes.
    tTimeIn=(quint32)this->m_CompressedData[tOffset+0]<<16|this->m_CompressedData[tOffset+1]<<8|this->m_CompressedData[tOffset+2]<<0;
    tOffset+=sizeof(quint8)*3;
    //timeout,3 bytes.
    tTimeOut=(quint32)this->m_CompressedData[tOffset+0]<<16|this->m_CompressedData[tOffset+1]<<8|this->m_CompressedData[tOffset+2]<<0;
    tOffset+=sizeof(quint8)*3;
    //Subtitle total frames number,1 byte.
    tSubtitleTotalNo=(quint8)this->m_CompressedData[tOffset+0];
    tOffset+=sizeof(quint8)*1;
    //Subtitle current frames number,1 byte.
    tSubtitleCurrentNo=(quint8)this->m_CompressedData[tOffset+0];
    tOffset+=sizeof(quint8)*1;
    //Total Alpha value.
    tOffset+=sizeof(quint8)*1;
    //Reserved 1 byte.
    tOffset+=sizeof(quint8)*1;
    //FadeUpEnd,4 bytes.
    tFadeUpEnd=(quint32)this->m_CompressedData[tOffset+0]<<24|this->m_CompressedData[tOffset+1]<<16|this->m_CompressedData[tOffset+2]<<8|this->m_CompressedData[tOffset+3]<<0;
    tOffset+=sizeof(quint8)*4;
    //FadeDownStart,4 bytes.
    tFadeDownStart=(quint32)this->m_CompressedData[tOffset+0]<<24|this->m_CompressedData[tOffset+1]<<16|this->m_CompressedData[tOffset+2]<<8|this->m_CompressedData[tOffset+3]<<0;
    tOffset+=sizeof(quint8)*4;
    //data length,4 bytes.
    tDataLength=(quint32)this->m_CompressedData[tOffset+0]<<24|this->m_CompressedData[tOffset+1]<<16|this->m_CompressedData[tOffset+2]<<8|this->m_CompressedData[tOffset+3]<<0;
    tOffset+=sizeof(quint8)*4;
    //data body.
    this->m_DataBodyStart=tOffset;
#if 1
    qDebug()<<"SpotNumber:"<<this->tSpotNumber;
    qDebug()<<"LeftTop:(x,y):"<<this->tLeftTopX<<this->tLeftTopY;
    qDebug()<<"Size:(width*height):"<<this->tPicWidth<<this->tPicHeight;
    qDebug()<<"TimeIn:"<<this->tTimeIn<<",TimeOut:"<<this->tTimeOut;
    qDebug()<<"Total:"<<this->tSubtitleTotalNo<<",Current:"<<this->tSubtitleCurrentNo;
    qDebug()<<"FadeUpEnd:"<<this->tFadeUpEnd;
    qDebug()<<"FadeDownStart:"<<this->tFadeDownStart;
    qDebug()<<"Compressed Data Length:"<<this->tDataLength;
#endif

    /*parse all elements ok,do decompress!*/
    if(true==this->ZFunctionDeCompress())
    {
        this->m_drawType=ZDraw_Type_Normal;
        this->update();
    }

    //add frame element to list.
    FrameElement *tFrameEle=new FrameElement;
    tFrameEle->ZFunctionSetSpotNumber(this->tSpotNumber);
    tFrameEle->ZFunctionSetLeftTopXY(this->tLeftTopX,this->tLeftTopY);
    tFrameEle->ZFunctionSetSize(this->tPicWidth,this->tPicHeight);
    tFrameEle->ZFunctionSetSubtitleFramesNumber(this->tSubtitleTotalNo,this->tSubtitleCurrentNo);
    this->m_frameElementList.append(tFrameEle);
}
void ScreenSimulator::paintEvent(QPaintEvent *event)
{
    QPainter winPainter(this);
    switch(this->m_drawType)
    {
    case ZDraw_Type_ClearScreen:
    {
        QRect winRect(0,0,this->size().width(),this->size().height());
        QBrush winBrush(QColor(0,0,0));
        winPainter.fillRect(winRect,winBrush);
    }
        break;
    case ZDraw_Type_Normal:
    {
        //draw the out border.
        winPainter.setPen(QPen(QBrush(QColor(0,255,0)),2));
        winPainter.drawRect(QRect(0,0,this->size().width(),this->size().height()));
        if(this->m_fileName.isEmpty())
        {
            return;
        }
        if(this->m_PixelDataLen<=0)
        {
            return;
        }
        QImage tImage(tPicWidth,tPicHeight,QImage::Format_ARGB32);
        quint8 *pixelData=this->m_PixelData;
        qDebug()<<"draw picture:"<<tPicWidth<<tPicHeight;
        quint32 offsetIndex=0;
        for(quint32 i=0;i<this->tPicHeight;i++)
        {
            for(quint32 j=0;j<this->tPicWidth;j++)
            {
                //AARRGGBB.
                //quint8 alpha=(quint8)(*(pixelData+i*tPicWidth*sizeof(quint32)+j*sizeof(quint32)+0));
                //quint8 red=(quint8)(*(pixelData+i*tPicWidth*sizeof(quint32)+j*sizeof(quint32)+1));
                //quint8 green=(quint8)(*(pixelData+i*tPicWidth*sizeof(quint32)+j*sizeof(quint32)+2));
                //quint8 blue=(quint8)(*(pixelData+i*tPicWidth*sizeof(quint32)+j*sizeof(quint32)+3));
                quint8 alpha=*(pixelData+offsetIndex+0);
                quint8 red=*(pixelData+offsetIndex+1);
                quint8 green=*(pixelData+offsetIndex+2);
                quint8 blue=*(pixelData+offsetIndex+3);
                tImage.setPixel(j,i,qRgba(red,green,blue,alpha));
                //qDebug("%02x %02x %02x %02x\n",alpha,red,green,blue);
                offsetIndex+=sizeof(qint32);
            }
        }
        //draw on the image.
        QPainter tPainter(this->mScreenImage);
        QRectF target(this->tLeftTopX,this->tLeftTopY,tPicWidth,tPicHeight);
        QRectF source(0,0,tPicWidth,tPicHeight);
        tPainter.drawPixmap(target,QPixmap::fromImage(tImage),source);


        //draw the image border with red color.
        if(this->m_bDrawOutline)
        {
            tPainter.setPen(QPen(QBrush(QColor(255,0,0)),2));
            tPainter.drawRect(QRect(this->tLeftTopX,this->tLeftTopY,tPicWidth,tPicHeight));
            this->m_bDrawOutline=0;
        }
        //draw to the window.
        winPainter.drawImage(0,0,*this->mScreenImage);
    }
        break;
    case ZDraw_Type_NetworkPlayer:
    {
        winPainter.drawImage(this->m_networkImageLeftTopX,this->m_networkImageLeftTopY,this->m_networkImage);
    }
        break;
    }
    return;
#if 0
    if(ZDraw_Type_ClearScreen==this->m_drawType)
    {
        QRect winRect(0,0,this->size().width(),this->size().height());
        QBrush winBrush(QColor(0,0,0));
        winPainter.fillRect(winRect,winBrush);
    }
    else if(ZDraw_Type_Normal==this->m_drawType)
    {
        //draw the out border.
        winPainter.setPen(QPen(QBrush(QColor(0,255,0)),2));
        winPainter.drawRect(QRect(0,0,this->size().width(),this->size().height()));
        if(this->m_fileName.isEmpty())
        {
            return;
        }
        if(this->m_PixelDataLen<=0)
        {
            return;
        }
        QImage tImage(tPicWidth,tPicHeight,QImage::Format_ARGB32);
        quint8 *pixelData=this->m_PixelData;
        qDebug()<<"draw picture:"<<tPicWidth<<tPicHeight;
        quint32 offsetIndex=0;
        for(quint32 i=0;i<this->tPicHeight;i++)
        {
            for(quint32 j=0;j<this->tPicWidth;j++)
            {
                //AARRGGBB.
                //quint8 alpha=(quint8)(*(pixelData+i*tPicWidth*sizeof(quint32)+j*sizeof(quint32)+0));
                //quint8 red=(quint8)(*(pixelData+i*tPicWidth*sizeof(quint32)+j*sizeof(quint32)+1));
                //quint8 green=(quint8)(*(pixelData+i*tPicWidth*sizeof(quint32)+j*sizeof(quint32)+2));
                //quint8 blue=(quint8)(*(pixelData+i*tPicWidth*sizeof(quint32)+j*sizeof(quint32)+3));
                quint8 alpha=*(pixelData+offsetIndex+0);
                quint8 red=*(pixelData+offsetIndex+1);
                quint8 green=*(pixelData+offsetIndex+2);
                quint8 blue=*(pixelData+offsetIndex+3);
                tImage.setPixel(j,i,qRgba(red,green,blue,alpha));
                //qDebug("%02x %02x %02x %02x\n",alpha,red,green,blue);
                offsetIndex+=sizeof(qint32);
            }
        }
        //draw on the image.
        QPainter tPainter(this->mScreenImage);
        QRectF target(this->tLeftTopX,this->tLeftTopY,tPicWidth,tPicHeight);
        QRectF source(0,0,tPicWidth,tPicHeight);
        tPainter.drawPixmap(target,QPixmap::fromImage(tImage),source);


        //draw the image border with red color.
        if(this->m_bDrawOutline)
        {
            tPainter.setPen(QPen(QBrush(QColor(255,0,0)),2));
            tPainter.drawRect(QRect(this->tLeftTopX,this->tLeftTopY,tPicWidth,tPicHeight));
            this->m_bDrawOutline=0;
        }
        //draw to the window.
        winPainter.drawImage(0,0,*this->mScreenImage);
    }else if(ZDraw_Type_NetworkPlayer==this->m_drawType)
    {
        winPainter.drawImage(this->m_networkImageLeftTopX,this->m_networkImageLeftTopY,this->m_networkImage);
    }
#endif
    return;
}

void ScreenSimulator::mouseMoveEvent(QMouseEvent *event)
{
    //qDebug()<<event->pos();
    QPoint tCurPos=event->pos();
    for(quint32 i=0;i<this->m_frameElementList.count();i++)
    {
        quint32 tX,tY;
        quint32 tWidth,tHeight;
        this->m_frameElementList.at(i)->ZFunctionGetLeftTopXY(&tX,&tY);
        this->m_frameElementList.at(i)->ZFunctionGetSize(&tWidth,&tHeight);
        if(tCurPos.x()>=tX &&tCurPos.x()<=tX+tWidth&& tCurPos.y()>=tY && tCurPos.y()<=tY+tHeight)
        {
            //display on the right side.
            QPoint displayPos;
            displayPos.setX(tX+tWidth);
            displayPos.setY(tY+tHeight);
            this->m_frameElementList.at(i)->move(displayPos);
            this->m_frameElementList.at(i)->show();
        }else{
            this->m_frameElementList.at(i)->close();
        }
    }
}

void ScreenSimulator::ZSlotDrawRGBStream(quint32 width,quint32 height,QString fileName)
{
    qDebug()<<width<<height<<fileName;
}

void ScreenSimulator::ZSlotDrawSubtitleImage(QImage image,quint32 leftTopX,quint32 leftTopY)
{
    this->m_drawType=ZDraw_Type_NetworkPlayer;
    this->m_networkImage=image;
    this->m_networkImageLeftTopX=leftTopX;
    this->m_networkImageLeftTopY=leftTopY;
    qDebug()<<"ScreenSimulator:";
    qDebug()<<this->m_networkImage.size();
    this->update();
}
