#-------------------------------------------------
#
# Project created by QtCreator 2011-09-16T18:42:08
#
#-------------------------------------------------

QT       += core gui network

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

win32 {
    INCLUDEPATH += d:/dev/miniupnpc
}
else {
    INCLUDEPATH += /usr/include/miniupnpc
}

LIBS += -lminiupnpc

win32 {
    LIBS += -lws2_32 -liphlpapi
}



#CONFIG += staticlib link_prl





