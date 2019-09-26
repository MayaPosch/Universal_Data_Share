/*
    mainwindow.h - declarations for Universal Data Share
    
    Revision 0
    
    Notes:
            - 
            
    2011/09/16, Maya Posch
    (c) Nyanko.ws
*/


#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#include <QMainWindow>
#include <QFile>
#include <QDir>
#include <QTcpServer>
#include <QNetworkAccessManager>
#include <QTcpSocket>
#include <QThread>
#include "ui_mainwindow.h"

#define STATICLIB
//#define _WIN32_WINNT 0x0601

#include <miniupnpc/miniupnpc.h>

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow, private Ui::MainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
public slots:
    void addFile();
    void removeFile();
    void quit();
    void copyGlobalURL();
    void downloadFile();
    void goOnline();
    void goOffline();
    void about();
    
    void loadAddress();
    void sendData();
    void socketError(QAbstractSocket::SocketError);
    void errorString(QString err);
    void refreshAddress();
    void receiveData();
    void incomingConnection();
    void serverRead();
    
signals:
    //void startDownload();

private:
    //Ui::MainWindow *ui;
            
    UPNPUrls urls;
    IGDdatas data;
    QString externalIP;
    QTcpServer* tcpserver;
    QTcpSocket* cSocket;            // client socket
    QTcpSocket* sSocket;
    //QList<QTcpSocket*> sSockets;    // server sockets
    
    bool online;
    QList<QFile*> files;
    QString address; // remote address
    QDir openDir;
};

#endif // MAINWINDOW_H
