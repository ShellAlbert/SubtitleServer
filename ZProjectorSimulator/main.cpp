#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //load qss file.
    QFile qssFile(":/skin/default.qss");
    if(qssFile.open(QFile::ReadOnly))
    {
        QString qssStr=qssFile.readAll();
        a.setStyleSheet(qssStr);
    }
    MainWindow w;
    w.show();

    return a.exec();
}
