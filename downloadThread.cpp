/*
    downloadThread.cpp - Download thread implementation.
    
    Revision 0
    
    Features:
                - 
                
    Notes:
                - 
                
    2011/10/24, Maya Posch
    (c) Nyanko.ws
*/

#include "downloadThread.h"

#include <QFile>
#include <QCryptographicHash>


// --- CONSTRUCTOR ---
DownloadThread::DownloadThread(QByteArray &data, QString f, QString a, quint16 p) {   
    //moveToThread(this);    
    qDebug("Hooked up new thread.");    
    msg = data;
    filepath = f;
    address = a;
    port = p;
}


// --- DECONSTRUCTOR ---
DownloadThread::~DownloadThread() {
    delete cSocket;
}


// --- CONNECT ---
// Main function of the thread. Gets executed when the thread launches.
void DownloadThread::connectHost() {
    // Send the provided message to remote after connecting 
    downloadStarted = false;
    cSocket = new QTcpSocket();
    connect(cSocket, SIGNAL(readyRead()), this, SLOT(receiveData()));
    connect(cSocket, SIGNAL(connected()), this, SLOT(sendData()));
    connect(cSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(socketError(QAbstractSocket::SocketError)));
    qDebug("Connecting to host...");
    cSocket->connectToHost(address, port);
    qDebug("Connected.");
}


// --- SEND DATA ---
// Send data to remote.
void DownloadThread::sendData() {   
    qDebug("Sending data...");
    qint64 written = 0;
    while (written != msg.size()) {
        written = cSocket->write(msg);
        if (written == -1) {
            emit error("Error writing data to socket: " + cSocket->errorString());
            cSocket->close();
            delete cSocket;
            emit finished();
        }
    }
    qDebug("Sent.");
}


// --- RECEIVE DATA ---
// Receive data from remote.
void DownloadThread::receiveData() {
    qDebug("Receiving Data...");
    // read the data length header first. This is an unsigned 32-bit integer.
    // Any additional data read here is a bonus.
    while (data.size() < 4) {
        data += cSocket->readAll();
    }
    
    QByteArray temp = data.mid(0, 4);
    quint32 datasize = *(quint32*)(temp.data());
    
    qDebug("datasize: " + QString::number(datasize).toAscii());
    qDebug("Data size " + QString::number(data.size()).toAscii());
    qDebug("cSocket size: " + QString::number(cSocket->bytesAvailable()).toAscii());
    
    // try to read until the full message size has been read.
    while (data.size() < datasize) {
        if (cSocket->waitForReadyRead(5000)) {
            data += cSocket->readAll();
            
            // update progress signal
            //qDebug("Progress: " + QString::number(data.size()).toAscii());
            //emit progress((int) (data.size() / datasize * 100));
        }
        else { break; }
    }
    
    qDebug("Data size " + QString::number(data.size()).toAscii());
        
    if (data.size() < 1 || data.size() < datasize) {
        emit error("Failed to read the response data.");
        emit finished();
        return;
    }
    
    // successful reception lsdmt.ath.cx
    
    
    // parse the received data.
    qDebug("Parsing header...");
    quint32 offset;
    //quint32 size = data.mid(0, 4).toInt();
    if (data.mid(4, 6) != "NYANKO") { return; }
    if (data.mid(10, 3) != "UDS") { return; }
    // TODO: check protocol version here. quint8
    if (data.mid(14, 4) == "FILE") {
        if (downloadStarted) { 
            emit error("Received FILE command with download in progress.");
            emit finished(); 
            return;
        }
        
        // Received a FILE response. Save the file somewhere.
        // read the default filename and verify hash.
        // Remove everything up till the file data start.
        offset = 18;
        QByteArray intbytes;
        intbytes = data.mid(offset, 8);
        offset += 8;
        filesize = *((quint64*) (intbytes.data()));
        intbytes = data.mid(offset, 4);
        offset += 4;
        quint32 length;
        length = *((quint32*) (intbytes.data()));
        //QString filename(data.mid(offset, length));
        offset += length;
        intbytes = data.mid(offset, 4);
        offset += 4;
        segments = *((quint32*) (intbytes.data())); // # of segments, 0-index.
        currentSegment = 0;
        QByteArray hash = data.mid(offset, 20);
        offset += 20;
        data.remove(0, offset);
        QByteArray filedata = data.mid(0, datasize - offset);
        QCryptographicHash hasher(QCryptographicHash::Sha1);
        hasher.addData(filedata);
        if (hasher.result() != hash) {
            // Corrupted download.
            qDebug("File hash: " + QString(hash).toAscii());
            qDebug("New hash: " + QString(hasher.result()).toAscii());
            emit error("Hash failure; possibly corrupted download.");
            emit finished();
            return; 
        }
        
        qDebug("Writing " + QString::number(filedata.size()).toAscii() + " bytes to file...");
        file.setFileName(filepath);
        file.open(QIODevice::WriteOnly);
        file.write(filedata);
        if (currentSegment >= segments) {
            file.close(); // we're done here
            emit progress(100);
            emit finished();
            return;
        }
        
        emit progress(filesize / bytesWritten * 100);
        data.remove(0, datasize - offset); // remove all read data.
        downloadStarted = true; // we expect more segments
        ++currentSegment;
    }
    else if (data.mid(14, 4) == "HASH") {
        offset = 18;
        QByteArray intbytes;
        intbytes = data.mid(offset, 4);
        offset += 4;
        quint32 segcount;
        segcount = *((quint32*) (intbytes.data()));
        if (segcount != (currentSegment + 1)) { 
            emit error("Next segment count is " + QString::number(segcount) 
                       + " but should be " + QString::number(currentSegment + 1));
            emit finished();
            return;
        }
        
        QByteArray hash = data.mid(offset, 20);
        offset += 20;
        data.remove(0, offset);
        QByteArray filedata = data.mid(0, datasize - offset);
        QCryptographicHash hasher(QCryptographicHash::Sha1);
        hasher.addData(filedata);
        if (hasher.result() != hash) {
            // Corrupted download.
            qDebug("File hash: " + QString(hash).toAscii());
            qDebug("New hash: " + QString(hasher.result()).toAscii());
            emit error("Hash failure; possibly corrupted download.");
            emit finished();
            return; 
        }
        
        qDebug("Writing " + QString::number(filedata.size()).toAscii() + " bytes to file...");
        file.write(filedata);
        if (currentSegment >= segments) {
            qDebug("Written. Emitting finished().");
            file.close(); // we're done here
            emit progress(100); // just to make sure :)
            emit finished();
            return;
        }
        
        qDebug("Written. Continuing with next segment.");
        ++currentSegment;
        emit progress(filesize / bytesWritten * 100);
        data.remove(0, datasize - offset); // remove all read data.
    }
    else {
        emit error("Received unknown response.");
        qDebug("Unknown response.");
        emit finished();
    }
    
    emit finished();
}


// --- SOCKET ERROR ---
// Called when a socket error occurs.
void DownloadThread::socketError(QAbstractSocket::SocketError) {
    if (cSocket->error() == QAbstractSocket::RemoteHostClosedError) { return; }
    emit error("Socket error: " + cSocket->errorString());
    //cSocket->close();
    //delete cSocket;
    emit finished(); // stop thread
}
