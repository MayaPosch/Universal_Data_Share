/*
    uploadThread.cpp - implementation for Universal Data Share's upload thread.
    
    Revision 0
    
    Notes:
            - 
            
    2011/11/06, Maya Posch
    (c) Nyanko.ws
*/

#include "uploadThread.h"

#include <QFileInfo>
#include <QCryptographicHash>


// --- CONSTRUCTOR ---
UploadThread::UploadThread(QFile* f, QTcpSocket *s) {
    file = f;
    sSocket = s;
}


// --- DECONSTRUCTOR ---
UploadThread::~UploadThread() {
    //
}


// --- SEND DATA ---
void UploadThread::sendData() {
    QByteArray output;
    output = "NYANKOUDS";
    quint8 revision = 0;
    output.append((char) revision);        
    output += "FILE";
    
    //qDebug("FILE id: " + QString::number(id).toLocal8Bit());
    
    file->open(QIODevice::ReadOnly);
    quint64 filesize = file->size();
    output.append((char*) &filesize, sizeof(filesize));
    
    QFileInfo fileInf = file->fileName();
    QByteArray filename = fileInf.fileName().toLocal8Bit();
    quint32 length = filename.size();
    output.append((char*) &length, sizeof(length)); // size of file in bytes
    
    output.append(filename);
    
    // start segment stuff
    // We use 1 MB segments. Determine the number of segments, then loop
    // through the open file and send HASH messages to client until all
    // bytes have been sent.
    quint32 segments = filesize / 1048576;
    if ((filesize % 1048576) > 0) { ++segments; }
    output.append((char*) &segments, sizeof(segments));
    
    QCryptographicHash hasher(QCryptographicHash::Sha1);
    QByteArray segment = file->read(1048576); // 1 MB in bytes
    hasher.addData(segment);
    output.append(hasher.result());
    output.append(segment);
            
    QByteArray lba;
    length = output.size() + sizeof(length);
    lba.append((char*) &length, sizeof(length));
    
    qDebug("Data length: " + QString::number(length).toLocal8Bit());
    
    output.prepend(lba); // data length prefix
    
    // write to client socket
    qint64 written = 0;
    while (written != output.size()) {
        written = sSocket->write(output);
        if (written == -1) {
            emit error("Error writing data to server socket: " + sSocket->errorString());
            break;
        }
    }
    
    // start the loop here. We start with the second segment, #1. We end when
    // all segments have been sent. Compose HAH message, send and repeat.
    for (quint32 i = 1; i < segments; ++i) {
        output = "NYANKOUDS";
        output.append((char) revision);        
        output += "HASH";        
        output.append((char*) &i, sizeof(i));
        segment = file->read(1048576); // 1 MB in bytes
        hasher.reset();
        hasher.addData(segment);
        output.append(hasher.result());
        output.append(segment);
        
        lba.clear();
        length = output.size() + sizeof(length);
        lba.append((char*) &length, sizeof(length));
        output.prepend(lba); // data length prefix
        
        // write to client socket
        qint64 written = 0;
        while (written != output.size()) {
            written = sSocket->write(output);
            if (written == -1) {
                emit error("Error writing data to server socket: " + sSocket->errorString());
                break;
            }
        }
    }
    
    file->close();
}


// --- SOCKET ERROR ---
// Called when a socket error occurs.
void UploadThread::socketError(QAbstractSocket::SocketError) {
    if (sSocket->error() == QAbstractSocket::RemoteHostClosedError) { return; }
    emit error("Socket error: " + sSocket->errorString());
    emit finished(); // stop thread
}
