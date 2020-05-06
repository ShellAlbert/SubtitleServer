#include "staticframewindow.h"
#include <QFileDialog>
#include <QDebug>
#include <QFileInfoList>
#include <QHeaderView>
StaticFrameWindow::StaticFrameWindow(QWidget *parent) :
    QWidget(parent)
{
    //create left .
    this->m_btnDirSelect=new QPushButton;
    this->m_btnDirSelect->setText(tr("Select Dir..."));
    connect(this->m_btnDirSelect,SIGNAL(clicked()),this,SLOT(ZSlotScanDirectory()));
    this->m_btnClearScreen=new QPushButton;
    this->m_btnClearScreen->setText(tr("Clear Screen"));
    connect(this->m_btnClearScreen,SIGNAL(clicked()),this,SIGNAL(ZSignalClearScreen()));

    //auto clean check box.
    this->m_checkBoxAutoClean=new QCheckBox;
    this->m_checkBoxAutoClean->setText(tr("Auto Clean"));
    //draw outline.
    this->m_checkBoxDrawOutline=new QCheckBox;
    this->m_checkBoxDrawOutline->setText(tr("Draw Outline"));
    //auto play.
    this->m_labelAutoPlay=new QLabel;
    this->m_labelAutoPlay->setText(tr("Auto Play"));
    this->m_spinBoxInterval=new QSpinBox;
    this->m_spinBoxInterval->setRange(100,10000);
    this->m_labelPlayUnit=new QLabel;
    this->m_labelPlayUnit->setText(tr("(ms)"));
    this->m_btnAutoPlay=new QPushButton;
    this->m_btnAutoPlay->setText(tr("Start"));
    connect(this->m_btnAutoPlay,SIGNAL(clicked()),this,SLOT(ZSlotAutoPlay()));
    this->m_autoPlayIndex=0;
    this->m_timerAutoPlay=new QTimer;


    //layout.
    this->m_hlayoutBtns=new QHBoxLayout;
    this->m_hlayoutBtns->addWidget(this->m_btnDirSelect);
    this->m_hlayoutBtns->addWidget(this->m_btnClearScreen);
    this->m_hlayoutBtns->addStretch(1);
    this->m_hlayoutBtns->addWidget(this->m_checkBoxAutoClean);
    this->m_hlayoutBtns->addWidget(this->m_checkBoxDrawOutline);
    this->m_hlayoutBtns->addWidget(this->m_labelAutoPlay);
    this->m_hlayoutBtns->addWidget(this->m_spinBoxInterval);
    this->m_hlayoutBtns->addWidget(this->m_labelPlayUnit);
    this->m_hlayoutBtns->addWidget(this->m_btnAutoPlay);
    //bottom part.
    this->m_tableWidget=new QTableWidget;
    this->m_tableWidget->setColumnCount(2);
    this->m_tableWidget->setRowCount(0);
    connect(this->m_tableWidget,SIGNAL(cellClicked(int,int)),this,SLOT(ZSlotDrawSubtitle(int,int)));
    QStringList strList;
    strList.append(tr("File"));
    strList.append(tr("Path"));
    this->m_tableWidget->setHorizontalHeaderLabels(strList);
    this->m_tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    this->m_tableWidget->horizontalHeader()->setStretchLastSection(true);

    //layout in main.
    this->m_vlayoutMain=new QVBoxLayout;
    this->m_vlayoutMain->addLayout(this->m_hlayoutBtns);
    this->m_vlayoutMain->addWidget(this->m_tableWidget);
    this->setLayout(this->m_vlayoutMain);
}
StaticFrameWindow::~StaticFrameWindow()
{
    delete this->m_btnDirSelect;
    delete this->m_btnClearScreen;
    delete this->m_checkBoxAutoClean;
    delete this->m_checkBoxDrawOutline;
    //auto play.
    delete this->m_labelAutoPlay;
    delete this->m_spinBoxInterval;
    delete this->m_labelPlayUnit;
    delete this->m_btnAutoPlay;
    delete this->m_timerAutoPlay;
    delete this->m_hlayoutBtns;
    delete this->m_tableWidget;
    delete this->m_vlayoutMain;
}
void StaticFrameWindow::ZSlotScanDirectory()
{
    QFileDialog fileDia;
    fileDia.setFileMode(QFileDialog::Directory);
    if(fileDia.exec()==QDialog::Accepted)
    {
        this->m_tableWidget->clearContents();
        this->m_tableWidget->setRowCount(0);
        QString dirName=fileDia.getExistingDirectory();
        qDebug()<<dirName;
        QDir tDir(dirName);
        QFileInfoList tList=tDir.entryInfoList();
        for(qint32 i=0;i<tList.count();i++)
        {
            //do not handle sub directory.
            if(tList.at(i).isDir())
            {
                continue;
            }
            //get file name.
            QString tFileName=tList.at(i).fileName();
            QString tPathName=tList.at(i).absoluteFilePath();
            //bypass . and ..
            if(tFileName=="." || tFileName=="..")
            {
                continue;
            }
            //the suffix must be .zhex.
            if(!tFileName.endsWith(".hex"))
            {
                continue;
            }
            qint32 rowNo=this->m_tableWidget->rowCount();
            this->m_tableWidget->insertRow(rowNo);
            this->m_tableWidget->setItem(rowNo,0,new QTableWidgetItem(tFileName));
            this->m_tableWidget->setItem(rowNo,1,new QTableWidgetItem(tPathName));
        }
        this->m_tableWidget->resizeColumnsToContents();
        this->m_tableWidget->resizeRowsToContents();
    }
}

 void StaticFrameWindow::ZSlotDrawSubtitle(int x,int y)
 {
     if(this->m_checkBoxAutoClean->isChecked())
     {
         emit this->ZSignalClearScreen();
     }
    QString fileName=this->m_tableWidget->item(x,1)->text();
    qDebug()<<fileName;
    if(fileName.isEmpty())
    {
        return;
    }
    if(this->m_checkBoxDrawOutline->isChecked())
    {
        emit this->ZSignalSubtitleFileName(fileName,1);
    }else{
        emit this->ZSignalSubtitleFileName(fileName,0);
    }
 }

 void StaticFrameWindow::ZSlotAutoPlay()
 {
    if(this->m_tableWidget->rowCount()<=0)
    {
        return;
    }
    this->m_autoPlayIndex=0;
    connect(this->m_timerAutoPlay,SIGNAL(timeout()),this,SLOT(ZSlotAutoPlayTimeout()));
    this->m_timerAutoPlay->setInterval(this->m_spinBoxInterval->value());
    this->m_timerAutoPlay->start();
    this->m_btnAutoPlay->setEnabled(false);
 }
void StaticFrameWindow::ZSlotAutoPlayTimeout()
{
    if(this->m_checkBoxAutoClean->isChecked())
    {
        emit this->ZSignalClearScreen();
    }
    QString fileName=this->m_tableWidget->item(this->m_autoPlayIndex,1)->text();
    if(this->m_checkBoxDrawOutline->isChecked())
    {
        emit this->ZSignalSubtitleFileName(fileName,1);
    }else{
        emit this->ZSignalSubtitleFileName(fileName,0);
    }
    this->m_autoPlayIndex++;
    if(this->m_autoPlayIndex>=this->m_tableWidget->rowCount())
    {
        this->m_timerAutoPlay->stop();
        disconnect(this->m_timerAutoPlay,SIGNAL(timeout()),this,SLOT(ZSlotAutoPlayTimeout()));
        this->m_btnAutoPlay->setEnabled(true);
    }
}
