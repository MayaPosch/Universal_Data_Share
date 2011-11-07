/*
    downloadThread.h - declarations for Universal Data Share's download thread.
    
    Revision 0
    
    Notes:
            - 
            
    2011/10/24, Maya Posch
    (c) Nyanko.ws
*/

#ifndef DOWNLOADTHREAD_H
#define DOWNLOADTHREAD_H

#include <QObject>
#include <QAbstractSocket>
#include <QTcpSocket>
#include <QFile>

class DownloadThread : public QObject {
    Q_OBJECT
    
public:
    DownloadThread(QByteArray &data, QString f, QString a, quint16 p);
    ~DownloadThread();
    
public slots:
    void connectHost();
    void sendData();
    void receiveData();
    void socketError(QAbstractSocket::SocketError);
    
signals:
    void finished();
    void progress(int percentage);
    void error(QString err);
    
private:    
    QTcpSocket* cSocket;
    QByteArray data;
    QByteArray msg;
    QString filepath;
    QString address;
    quint16 port;
    bool downloadStarted;
    QFile file;
    quint64 filesize;
    quint64 bytesWritten;
    quint32 segments;
    quint32 currentSegment;
};

#endif // DOWNLOADTHREAD_H
