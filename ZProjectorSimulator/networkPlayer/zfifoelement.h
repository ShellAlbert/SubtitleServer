#ifndef ZFIFOELEMENT_H
#define ZFIFOELEMENT_H

#include <QObject>

class ZFIFOElement : public QObject
{
    Q_OBJECT
public:
    explicit ZFIFOElement(QObject *parent = 0);
    ~ZFIFOElement();
public:
    qint32 ZFunctionDeCompress();
signals:

public slots:

public:
    //store compressed data.
    quint8  *m_buffer;
    quint32 m_totalBytes;

    //store decompressed data.
    quint8 *m_bufferDecompress;
    quint32 m_totalByteDecompress;

    //for protocol specification.
    quint32 m_SpotNumber;
    quint32 m_LeftTopX;
    quint32 m_LeftTopY;
    quint32 m_PicWidth;
    quint32 m_PicHeight;
    quint32 m_TimeIn;
    quint32 m_TimeOut;
    quint8  m_SubtitleTotalNumber;
    quint8  m_SubtitleCurrentNumber;
    quint8  m_FadeUpAlphaStep;
    quint8  m_FadeDownAlphaStep;
    quint32 m_FadeUpEnd;
    quint32 m_FadeDownStart;
    quint32 m_DataLength;
};

#endif // ZFIFOELEMENT_H
