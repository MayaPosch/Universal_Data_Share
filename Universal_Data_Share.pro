#-------------------------------------------------
#
# Project created by QtCreator 2011-09-16T18:42:08
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Universal_Data_Share
TEMPLATE = app

QMAKE_LFLAGS += -static-libgcc -static-libstdc++

SOURCES += main.cpp\
        mainwindow.cpp \
    downloadThread.cpp \
    uploadThread.cpp

HEADERS  += mainwindow.h \
    downloadThread.h \
    udsp.h \
    uploadThread.h

FORMS    += mainwindow.ui

LIBS += -lminiupnpc

win32 {
    LIBS += -lws2_32 -liphlpapi
}






