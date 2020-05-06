#include "rgbdrawer.h"
#include <QFileDialog>
RGBDrawer::RGBDrawer(QWidget *parent) :
    QWidget(parent)
{
    //width.
    this->m_labelWidth=new QLabel;
    this->m_labelWidth->setText(tr("Width"));
    this->m_lineEditWidth=new QLineEdit;
    //height.
    this->m_labelHeight=new QLabel;
    this->m_labelHeight->setText(tr("Height"));
    this->m_lineEditHeight=new QLineEdit;
    //layout
    this->m_hlayoutSize=new QHBoxLayout;
    this->m_hlayoutSize->addWidget(this->m_labelWidth);
    this->m_hlayoutSize->addWidget(this->m_lineEditWidth);
    this->m_hlayoutSize->addWidget(this->m_labelHeight);
    this->m_hlayoutSize->addWidget(this->m_lineEditHeight);

    //file select.
    this->m_labelFile=new QLabel;
    this->m_labelFile->setText(tr("File"));
    this->m_lineEditFile=new QLineEdit;
    this->m_btnBrowse=new ZButton;
    this->m_btnBrowse->setText(tr("Browse..."));
    connect(this->m_btnBrowse,SIGNAL(clicked()),this,SLOT(ZSlotBrowseLocalFile()));
    this->m_hlayoutFile=new QHBoxLayout;
    this->m_hlayoutFile->addWidget(this->m_labelFile);
    this->m_hlayoutFile->addWidget(this->m_lineEditFile);
    this->m_hlayoutFile->addWidget(this->m_btnBrowse);

    //draw button.
    this->m_btnDraw=new ZButton;
    this->m_btnDraw->setText(tr("Draw Now"));
    this->m_btnDraw->setToolTip(tr("Draw Now"));
    connect(this->m_btnDraw,SIGNAL(clicked()),this,SLOT(ZSlotDrawNow()));

    //layout in vertical.
    this->m_vlayoutMain=new QVBoxLayout;
    this->m_vlayoutMain->addLayout(this->m_hlayoutSize);
    this->m_vlayoutMain->addLayout(this->m_hlayoutFile);
    this->m_vlayoutMain->addWidget(this->m_btnDraw);
    this->setLayout(this->m_vlayoutMain);
}

RGBDrawer::~RGBDrawer()
{
    //size.
    delete this->m_labelWidth;
    delete this->m_lineEditWidth;
    delete this->m_labelHeight;
    delete this->m_lineEditHeight;
    delete this->m_hlayoutSize;
    //file
    delete this->m_labelFile;
    delete this->m_lineEditFile;
    delete this->m_btnBrowse;
    delete this->m_hlayoutFile;

    delete this->m_btnDraw;
    delete this->m_vlayoutMain;
}

void RGBDrawer::ZSlotBrowseLocalFile()
{
    QFileDialog fileDia;
    if(QDialog::Accepted==fileDia.exec())
    {
        this->m_lineEditFile->setText(fileDia.getOpenFileName());
    }
}

void RGBDrawer::ZSlotDrawNow()
{
    bool bOk;
    //check width.
    quint32 tWidth=this->m_lineEditWidth->text().toInt(&bOk);
    if(false==bOk)
    {
        this->m_lineEditWidth->setText(tr("Error Width"));
        this->m_lineEditWidth->setSelection(0,this->m_lineEditWidth->text().length());
        this->m_lineEditWidth->setFocus();
        return;
    }
    //check height.
    quint32 tHeight=this->m_lineEditHeight->text().toInt(&bOk);
    if(false==bOk)
    {
        this->m_lineEditHeight->setText(tr("Error Height"));
        this->m_lineEditHeight->setSelection(0,this->m_lineEditHeight->text().length());
        this->m_lineEditHeight->setFocus();
        return;
    }
    //check file.
    QString tFileName=this->m_lineEditFile->text();
    if(tFileName.isEmpty())
    {
        this->m_lineEditFile->setFocus();
        return;
    }
    //emit the signal.
    emit this->ZSignalRGBDrawer(tWidth,tHeight,tFileName);
    return;
}
