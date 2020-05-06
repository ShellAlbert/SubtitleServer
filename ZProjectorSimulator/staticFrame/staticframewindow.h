#ifndef STATICFRAMEWINDOW_H
#define STATICFRAMEWINDOW_H

#include <QWidget>
#include <QPushButton>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QCheckBox>
#include <QSpinBox>
#include <QLabel>
#include <QTimer>
#include <screenSimulator/screensimulator.h>
class StaticFrameWindow : public QWidget
{
    Q_OBJECT
public:
    explicit StaticFrameWindow(QWidget *parent = 0);
    ~StaticFrameWindow();


signals:
    void ZSignalClearScreen();
    void ZSignalSubtitleFileName(const QString fileName,quint8  bDrawOutline);
public slots:
    void ZSlotScanDirectory();
    void ZSlotDrawSubtitle(int x,int y);
    void ZSlotAutoPlay();
    void ZSlotAutoPlayTimeout();
private:
    //top part.
    QPushButton *m_btnDirSelect;
    QPushButton *m_btnClearScreen;
    QCheckBox  *m_checkBoxAutoClean;
    QCheckBox *m_checkBoxDrawOutline;
    //auto play.
    QLabel *m_labelAutoPlay;
    QSpinBox *m_spinBoxInterval;
    QLabel *m_labelPlayUnit;
    QPushButton *m_btnAutoPlay;
    QTimer *m_timerAutoPlay;
    quint32 m_autoPlayIndex;
    //layout.
    QHBoxLayout *m_hlayoutBtns;
    //bottom part.
    QTableWidget *m_tableWidget;
    //layout main.
    QVBoxLayout *m_vlayoutMain;
};

#endif // STATICFRAMEWINDOW_H
