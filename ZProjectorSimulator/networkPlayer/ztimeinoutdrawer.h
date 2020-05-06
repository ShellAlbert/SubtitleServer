#ifndef ZTIMEINOUTDRAWER_H
#define ZTIMEINOUTDRAWER_H

#include <QWidget>
#include <QPaintEvent>
class ZTimeInOutDrawer : public QWidget
{
    Q_OBJECT
public:
    explicit ZTimeInOutDrawer(QWidget *parent = 0);
    void ZFunctionDrawTimeInOut(quint32 fadeUpAlphaStep,quint32 fadeDownAlphaStep,quint32 timeIn,quint32 fadeUpEnd,quint32 fadeDownStart,quint32 timeOut,quint32 rate);
protected:
    void paintEvent(QPaintEvent *event);
    QSize sizeHint() const;
signals:

public slots:

private:
    quint32 m_FadeUpAlphaStep;
    quint32 m_FadeDownAlphaStep;
    quint32 m_TimeIn;
    quint32 m_FadeUpEnd;
    quint32 m_FadeDownStart;
    quint32 m_TimeOut;
    quint32 m_rate;
};

#endif // ZTIMEINOUTDRAWER_H
