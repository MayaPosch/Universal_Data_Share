/*
    uploadThread.h - declarations for Universal Data Share's upload thread.
    
    Revision 0
    
    Notes:
            - 
            
    2011/11/06, Maya Posch
    (c) Nyanko.ws
*/

#ifndef UPLOADTHREAD_H
#define UPLOADTHREAD_H

#include <QObject>
#include <QAbstractSocket>
#include <QTcpSocket>
#include <QFile>

class UploadThread : public QObject {
    Q_OBJECT
    
public:
    UploadThread(QFile* f, QTcpSocket* s);
    ~UploadThread();
    
public slots:
    void sendData();
    void socketError(QAbstractSocket::SocketError);
    
signals:
    void finished();
    void error(QString err);
    
private:    
    QTcpSocket* sSocket;
    QFile* file;
};

#endif // UPLOADTHREAD_H
