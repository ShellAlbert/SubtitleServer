#ifndef RGBDRAWER_H
#define RGBDRAWER_H

#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <zbutton/zbutton.h>
class RGBDrawer : public QWidget
{
    Q_OBJECT
public:
    explicit RGBDrawer(QWidget *parent = 0);
    ~RGBDrawer();
signals:
    void ZSignalRGBDrawer(quint32 width,quint32 height,QString fileName);
public slots:
    void ZSlotBrowseLocalFile();
    void ZSlotDrawNow();
private:
    //size.
    QLabel *m_labelWidth;
    QLineEdit *m_lineEditWidth;
    QLabel *m_labelHeight;
    QLineEdit *m_lineEditHeight;
    QHBoxLayout *m_hlayoutSize;

    //file.
    QLabel *m_labelFile;
    QLineEdit *m_lineEditFile;
    ZButton *m_btnBrowse;
    QHBoxLayout *m_hlayoutFile;

    //button.
    ZButton *m_btnDraw;
    QVBoxLayout *m_vlayoutMain;
};

#endif // RGBDRAWER_H
