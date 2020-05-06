#-------------------------------------------------
#
# Project created by QtCreator 2014-12-29T09:50:39
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ZProjectorSimulator
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    staticFrame/staticframewindow.cpp \
    screenSimulator/screensimulator.cpp \
    screenSimulator/frameelement.cpp \
    zbutton/zbutton.cpp \
    rgbDrawer/rgbdrawer.cpp \
    networkPlayer/znetworkplayer.cpp \
    networkPlayer/ztcpserver.cpp \
    networkPlayer/ztcpthread.cpp \
    networkPlayer/zfifoelement.cpp \
    networkPlayer/zdrawthread.cpp \
    networkPlayer/ztimeinoutdrawer.cpp

HEADERS  += mainwindow.h \
    staticFrame/staticframewindow.h \
    screenSimulator/screensimulator.h \
    screenSimulator/frameelement.h \
    zparam.h \
    zbutton/zbutton.h \
    rgbDrawer/rgbdrawer.h \
    networkPlayer/znetworkplayer.h \
    networkPlayer/ztcpserver.h \
    networkPlayer/ztcpthread.h \
    networkPlayer/zfifoelement.h \
    networkPlayer/zdrawthread.h \
    networkPlayer/ztimeinoutdrawer.h

OTHER_FILES += \
    default.qss

RESOURCES += \
    skin.qrc \
    images.qrc
