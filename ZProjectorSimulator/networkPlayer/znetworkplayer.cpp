#include "znetworkplayer.h"
#include <QHeaderView>
ZNetworkPlayer::ZNetworkPlayer(QWidget *parent) :
    QWidget(parent)
{
     //buffer status.
    this->m_labelBuffer=new QLabel;
    this->m_labelBuffer->setText(tr("Buffer Pool"));
    this->m_progressBar=new QProgressBar;
    this->m_progressBar->setRange(0,100);
    this->m_tableWidget=new QTableWidget;
    this->m_tableWidget->setRowCount(0);
    this->m_tableWidget->setColumnCount(12);
    QStringList strList;
    strList.append(tr("SpotNumber"));
    strList.append(tr("(x,y)"));
    strList.append(tr("width*height"));
    strList.append(tr("Compressed Size"));
    strList.append(tr("Decompress Size"));
    strList.append(tr("Rate"));
    strList.append(tr("FadeUpAlphaStep"));
    strList.append(tr("FadeDownAlphaStep"));
    strList.append(tr("TimeIn"));
    strList.append(tr("FadeUpEnd"));
    strList.append(tr("FadeDownStart"));
    strList.append(tr("TimeOut"));
    this->m_tableWidget->setHorizontalHeaderLabels(strList);
    this->m_tableWidget->horizontalHeader()->setStretchLastSection(true);
    this->m_tableWidget->resizeColumnsToContents();
    this->m_tableWidget->resizeRowsToContents();


    //timein out drawre.
    this->m_timeInOutDrawer=new ZTimeInOutDrawer;

    //control buttons.
    this->m_labelControl=new QLabel;
    this->m_labelControl->setText(tr("Service Control"));
    this->m_btnStartStop=new QPushButton;
    this->m_btnStartStop->setText(tr("Start"));
    connect(this->m_btnStartStop,SIGNAL(clicked()),this,SLOT(ZSlotStartStopListen()));
    this->m_hlayoutBtn=new QHBoxLayout;
    this->m_hlayoutBtn->addWidget(this->m_labelControl);
    this->m_hlayoutBtn->addStretch(1);
    this->m_hlayoutBtn->addWidget(this->m_btnStartStop);

    //layout vertical.
    this->m_vlayoutMain=new QVBoxLayout;
    this->m_vlayoutMain->addWidget(this->m_labelBuffer);
    this->m_vlayoutMain->addWidget(this->m_progressBar);
    this->m_vlayoutMain->addWidget(this->m_tableWidget);
    this->m_vlayoutMain->addWidget(this->m_timeInOutDrawer);
    this->m_vlayoutMain->addLayout(this->m_hlayoutBtn);
    this->setLayout(this->m_vlayoutMain);

    //tcp server.
    this->m_tcpServer=new ZTCpServer;
    connect(this->m_tcpServer,SIGNAL(ZSignalClearScreen()),this,SIGNAL(ZSignalClearScreen()));
    connect(this->m_tcpServer,SIGNAL(ZSignalReceivedNewImage(QImage,quint32,quint32)),this,SIGNAL(ZSignalReceivedNewSubtitleImage(QImage,quint32,quint32)));
    connect(this->m_tcpServer,SIGNAL(ZSignalReceivedSubtitle(quint32,quint32,quint32,quint32,quint32,quint32,quint32,quint8,quint8,quint32,quint32,quint32,quint32)),
            this,SLOT(ZSlotAddNewSubtitleToTable(quint32,quint32,quint32,quint32,quint32,quint32,quint32,quint8,quint8,quint32,quint32,quint32,quint32)));

    //update progress bar.
    connect(this->m_tcpServer,SIGNAL(ZSignalSubtitleSize(quint32,quint32)),this,SLOT(ZSlotSetSubtitleSize(quint32,quint32)));
    connect(this->m_tcpServer,SIGNAL(ZSignalSubtitleCurrentPixel(quint32)),this,SLOT(ZSlotSetSubtitleCurrentPixel(quint32)));
}
ZNetworkPlayer::~ZNetworkPlayer()
{
    //buffer pool.
    delete this->m_labelBuffer;
    delete this->m_progressBar;
    delete this->m_tableWidget;
    delete this->m_timeInOutDrawer;
    //control buttons.
    delete this->m_labelControl;
    delete this->m_btnStartStop;
    delete this->m_hlayoutBtn;
    delete this->m_vlayoutMain;
    //tcp server.
    delete this->m_tcpServer;
}
void ZNetworkPlayer::ZSlotStartStopListen()
{
    if(this->m_tcpServer->isListening())
    {
        this->m_tcpServer->close();
        this->m_btnStartStop->setText(tr("Start"));
        return;
    }
    if(this->m_tcpServer->listen(QHostAddress::Any,1986))
    {
        this->m_btnStartStop->setText(tr("Stop"));
    }else{
        qDebug()<<"listen failed!";
    }
}
void ZNetworkPlayer::ZSlotAddNewSubtitleToTable(quint32 spotNumber,//
                                                quint32 x,quint32 y,//
                                                quint32 width,quint32 height,//
                                                quint32 compressSize,quint32 decompressSize,//
                                                quint8 fadeUpAlphaStep,quint8 fadeDownAlphaStep,//
                                                quint32 timeIn,quint32 fadeUp,//
                                                quint32 fadeDown,quint32 timeOut)
{
    int rowNum=this->m_tableWidget->rowCount();
    this->m_tableWidget->insertRow(rowNum);
    this->m_tableWidget->setItem(rowNum,0,new QTableWidgetItem(QString("%1").arg(spotNumber)));
    this->m_tableWidget->item(rowNum,0)->setTextAlignment(Qt::AlignCenter);
    this->m_tableWidget->setItem(rowNum,1,new QTableWidgetItem(QString("(%1,%2)").arg(x).arg(y)));
    this->m_tableWidget->item(rowNum,1)->setTextAlignment(Qt::AlignCenter);
    this->m_tableWidget->setItem(rowNum,2,new QTableWidgetItem(QString("%1*%2").arg(width).arg(height)));
    this->m_tableWidget->item(rowNum,2)->setTextAlignment(Qt::AlignCenter);
    this->m_tableWidget->setItem(rowNum,3,new QTableWidgetItem(QString("%1").arg(compressSize)));
    this->m_tableWidget->item(rowNum,3)->setTextAlignment(Qt::AlignCenter);
    this->m_tableWidget->setItem(rowNum,4,new QTableWidgetItem(QString("%1").arg(decompressSize)));
    this->m_tableWidget->item(rowNum,4)->setTextAlignment(Qt::AlignCenter);
    //Compressing Rate.
    quint32 rate=(float)compressSize/decompressSize*100.0;
    QString tRate;
    tRate.sprintf("%.2f%%",(float)compressSize/decompressSize*100.0);
    this->m_tableWidget->setItem(rowNum,5,new QTableWidgetItem(QString("%1").arg(tRate)));
    this->m_tableWidget->item(rowNum,5)->setTextAlignment(Qt::AlignCenter);
    //fade up alpha step.
    this->m_tableWidget->setItem(rowNum,6,new QTableWidgetItem(QString("%1").arg(fadeUpAlphaStep)));
    this->m_tableWidget->item(rowNum,6)->setTextAlignment(Qt::AlignCenter);
    //fade down alpha step.
    this->m_tableWidget->setItem(rowNum,7,new QTableWidgetItem(QString("%1").arg(fadeDownAlphaStep)));
    this->m_tableWidget->item(rowNum,7)->setTextAlignment(Qt::AlignCenter);
    //timeIn.
    this->m_tableWidget->setItem(rowNum,8,new QTableWidgetItem(QString("%1").arg(timeIn)));
    this->m_tableWidget->item(rowNum,8)->setTextAlignment(Qt::AlignCenter);
    //fadeUpEnd.
    this->m_tableWidget->setItem(rowNum,9,new QTableWidgetItem(QString("%1").arg(fadeUp)));
    this->m_tableWidget->item(rowNum,9)->setTextAlignment(Qt::AlignCenter);
    //fadeDownStart.
    this->m_tableWidget->setItem(rowNum,10,new QTableWidgetItem(QString("%1").arg(fadeDown)));
    this->m_tableWidget->item(rowNum,10)->setTextAlignment(Qt::AlignCenter);
    //timeOut.
    this->m_tableWidget->setItem(rowNum,11,new QTableWidgetItem(QString("%1").arg(timeOut)));
    this->m_tableWidget->item(rowNum,11)->setTextAlignment(Qt::AlignCenter);
    this->m_tableWidget->scrollToBottom();

    //update the TimeInOut Drawer.
    this->m_timeInOutDrawer->ZFunctionDrawTimeInOut(fadeUpAlphaStep,fadeDownAlphaStep,timeIn,fadeUp,fadeDown,timeOut,rate);
}
void ZNetworkPlayer::ZSlotSetSubtitleSize(quint32 width,quint32 height)
{
    this->m_progressBar->setRange(0,width*height);
}
void ZNetworkPlayer::ZSlotSetSubtitleCurrentPixel(quint32 value)
{
    this->m_progressBar->setValue(value);
}
