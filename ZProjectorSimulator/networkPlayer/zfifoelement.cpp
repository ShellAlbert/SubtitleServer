#include "zfifoelement.h"
#include <QDebug>
ZFIFOElement::ZFIFOElement(QObject *parent) :
    QObject(parent)
{
    //for compressed data.
    this->m_buffer=new quint8[1*1024*1024];
    this->m_totalBytes=0;

    //for decompressed data.
    this->m_bufferDecompress=new quint8[1*1024*1024];
    this->m_totalByteDecompress=0;
}
ZFIFOElement::~ZFIFOElement()
{
    delete [] this->m_buffer;
    delete [] this->m_bufferDecompress;
}

qint32 ZFIFOElement::ZFunctionDeCompress()
{
    if(this->m_totalBytes<=0)
    {
        return -1;
    }
    //start.
    quint32 tOffset=0;
    //bypass 128-bit sync header.
    if(this->m_buffer[0]!=0xCC&&this->m_buffer[1]!=0xCC &&this->m_buffer[2]!=0xCC && this->m_buffer[3]!=0xCC)
    {
        return -1;
    }
    tOffset+=sizeof(quint8)*16;

    //SpotNumber,2 bytes.
    this->m_SpotNumber=(quint32)this->m_buffer[tOffset+0]<<8|this->m_buffer[tOffset+1]<<0;
    tOffset+=sizeof(quint8)*2;
    //LeftTopX,2 bytes.
    this->m_LeftTopX=(quint32)this->m_buffer[tOffset+0]<<8|this->m_buffer[tOffset+1]<<0;
    tOffset+=sizeof(quint8)*2;

    //LeftTopY,2 bytes.
    this->m_LeftTopY=(quint32)this->m_buffer[tOffset+0]<<8|this->m_buffer[tOffset+1]<<0;
    tOffset+=sizeof(quint8)*2;

    //picture width,2 bytes.
    this->m_PicWidth=(quint32)this->m_buffer[tOffset+0]<<8|this->m_buffer[tOffset+1]<<0;
    tOffset+=sizeof(quint8)*2;
    //picture height,2 bytes.
    this->m_PicHeight=(quint32)this->m_buffer[tOffset+0]<<8|this->m_buffer[tOffset+1]<<0;
    tOffset+=sizeof(quint8)*2;

    //timein,3 bytes.
    this->m_TimeIn=(quint32)this->m_buffer[tOffset+0]<<16|this->m_buffer[tOffset+1]<<8|this->m_buffer[tOffset+2]<<0;
    tOffset+=sizeof(quint8)*3;
    //timeout,3 bytes.
    this->m_TimeOut=(quint32)this->m_buffer[tOffset+0]<<16|this->m_buffer[tOffset+1]<<8|this->m_buffer[tOffset+2]<<0;
    tOffset+=sizeof(quint8)*3;

    //Subtitle total frames number,1 byte.
    this->m_SubtitleTotalNumber=(quint8)this->m_buffer[tOffset+0];
    tOffset+=sizeof(quint8)*1;

    //Subtitle current frames number,1 byte.
    this->m_SubtitleCurrentNumber=(quint8)this->m_buffer[tOffset+0];
    tOffset+=sizeof(quint8)*1;

    //fade up alpha step value..
    this->m_FadeUpAlphaStep=(quint8)this->m_buffer[tOffset+0];
    tOffset+=sizeof(quint8)*1;

    //fade down alpha step value.
    this->m_FadeDownAlphaStep=(quint8)this->m_buffer[tOffset+0];
    tOffset+=sizeof(quint8)*1;

    //FadeUpEnd,4 bytes.
    this->m_FadeUpEnd=(quint32)this->m_buffer[tOffset+0]<<24|this->m_buffer[tOffset+1]<<16|this->m_buffer[tOffset+2]<<8|this->m_buffer[tOffset+3]<<0;
    tOffset+=sizeof(quint8)*4;

    //FadeDownStart,4 bytes.
    this->m_FadeDownStart=(quint32)this->m_buffer[tOffset+0]<<24|this->m_buffer[tOffset+1]<<16|this->m_buffer[tOffset+2]<<8|this->m_buffer[tOffset+3]<<0;
    tOffset+=sizeof(quint8)*4;

    //data length,4 bytes.
    this->m_DataLength=(quint32)this->m_buffer[tOffset+0]<<24|this->m_buffer[tOffset+1]<<16|this->m_buffer[tOffset+2]<<8|this->m_buffer[tOffset+3]<<0;
    tOffset+=sizeof(quint8)*4;

#if 1
    qDebug()<<"SpotNumber:"<<this->m_SpotNumber;
    qDebug()<<"LeftTop:(x,y):"<<this->m_LeftTopX<<this->m_LeftTopY;
    qDebug()<<"Size:(width*height):"<<this->m_PicWidth<<this->m_PicHeight;
    qDebug()<<"TimeIn:"<<this->m_TimeIn<<",TimeOut:"<<this->m_TimeOut;
    qDebug()<<"Total:"<<this->m_SubtitleTotalNumber<<",Current:"<<this->m_SubtitleCurrentNumber;
    qDebug()<<"FadeUpEnd:"<<this->m_FadeUpEnd;
    qDebug()<<"FadeDownStart:"<<this->m_FadeDownStart;
    qDebug()<<"Compressed Data Length:"<<this->m_DataLength;
#endif

    //start to decompress.
    quint8 marker;
    quint32 i, j;
    quint32 len_decompress = 0;
    quint32 repeat_times = 0;

    //clear decompress buffer.
    this->m_totalByteDecompress=0;
    memset(this->m_bufferDecompress,0,1*1024*1024);

    quint8 *pSource=&this->m_buffer[tOffset];
    quint32 sourceLength=this->m_totalBytes;
    quint8 *pDest=this->m_bufferDecompress;

    //src_data[0]:common marker.
    marker = pSource[0];
    qDebug("Marker=0x%02x\n",marker);

    for (i = 1; i < sourceLength;)
    {
        if (pSource[i] == marker)
        {
            //compressed data.
            //repeat_times = pSource[i + 1];//old code.
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

    this->m_totalByteDecompress=len_decompress;
    qDebug()<<"len decompress:"<<m_totalByteDecompress;
}
