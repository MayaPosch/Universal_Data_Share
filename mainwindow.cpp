/*
    mainwindow.cpp - Main implementation file of Universal Data Share
    
    Revision 0
    
    Features:
                - 
                
    Notes:
                - 
                
    2011/09/16, Maya Posch
    2019/01/30, Maya Posch
    (c) Nyanko.ws
*/

#include "mainwindow.h"
#include "downloadThread.h"
#include "uploadThread.h"

#include <upnpcommands.h>
#include <upnperrors.h>

#ifdef WIN32
#include <winsock2.h>
#endif

#include <QMessageBox>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QFileDialog>
#include <QClipboard>
#include <QNetworkInterface>
#include <QNetworkAddressEntry>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QHostAddress>
#include <QThread>
#include <QProgressBar>
#include <QCryptographicHash>


// --- CONSTRUCTOR ---
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    setupUi(this);
    
    // set up menu connections
    actionAdd->setShortcuts(QKeySequence::New);
    connect(actionAdd, SIGNAL(triggered()), this, SLOT(addFile()));
    
    actionRemove->setShortcuts(QKeySequence::Delete);
    connect(actionRemove, SIGNAL(triggered()), this, SLOT(removeFile()));
    connect(actionCopy_Global_URL, SIGNAL(triggered()), this, SLOT(copyGlobalURL()));
    connect(actionGo_online, SIGNAL(triggered()), this, SLOT(goOnline()));
    connect(actionGo_offline, SIGNAL(triggered()), this, SLOT(goOffline()));
    
    actionDownload_selected->setShortcuts(QKeySequence::Save);
    connect(actionDownload_selected, SIGNAL(triggered()), this, SLOT(downloadFile()));
    connect(actionAbout, SIGNAL(triggered()), this, SLOT(about()));
    connect(actionExit, SIGNAL(triggered()), this, SLOT(quit()));
    
    actionGo_offline->setEnabled(false);
    
    connect(Goto, SIGNAL(pressed()), this, SLOT(loadAddress()));
    connect(refresh, SIGNAL(pressed()), this, SLOT(refreshAddress()));
    
#ifdef WIN32
    // network socket initialization
    WSADATA wsaData;
    int nResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if(nResult != NO_ERROR) {
        QMessageBox::critical(this, "Error", "WSAStartup() failed.");
        //return -1;
        return;
    } 
#endif
    
    tcpserver = new QTcpServer();
    connect(tcpserver, SIGNAL(newConnection()), this, SLOT(incomingConnection()));
    statusView->showMessage(tr("Offline"));
    
    cSocket = new QTcpSocket();
    connect(cSocket, SIGNAL(readyRead()), this, SLOT(receiveData()));
    connect(cSocket, SIGNAL(connected()), this, SLOT(sendData()));
    connect(cSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(socketError(QAbstractSocket::SocketError)));
    
    // defaults
    online = false;
    openDir.setPath(""); // empty
    statusView->clearMessage();
    statusView->showMessage(tr("Offline"));
}


// --- DECONSTRUCTOR ---
MainWindow::~MainWindow() {
    if (online) { goOffline(); }
    //delete ui;
    delete tcpserver;
    //delete netmanager;
    delete cSocket;
}


// --- ADD FILE ---
// Allows one to add a new file to the list.
void MainWindow::addFile() {
    QStringList filepaths = QFileDialog::getOpenFileNames(this, tr("Select Files"), openDir.absolutePath());
    if (filepaths.size() < 1) { return; }
    
    QFile* file;
    QList<QTreeWidgetItem*> items;
    QStringList row;
    for (int i = 0; i < filepaths.size(); ++i) {
        file = new QFile(filepaths.at(i));
        files.prepend(file);
        QFileInfo info(filepaths.at(i));
        row.clear();
        row << info.fileName() << QString::number(info.size() / 1024) + " kB" << info.path();
        items.append(new QTreeWidgetItem((QTreeWidgetItem*) 0, row));
    }
    
    localList->insertTopLevelItems(0, items);
}


// --- REMOVE FILE ---
// Remove the selected file from the list.
void MainWindow::removeFile() {
    int row = localList->indexOfTopLevelItem(localList->currentItem());
    QTreeWidgetItem* item = localList->takeTopLevelItem(row);
    if (item) { delete item; }
    //files.removeAt(row);
}


// --- COPY GLOBAL URL ---
// Copies the URL for the root shared folder to the clipboard.
void MainWindow::copyGlobalURL() {
    if (externalIP.size() < 1) { 
        QMessageBox::information(this, tr("Information"), tr("You need to be online to copy the external IP address"));
    }
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(externalIP);
}


// --- DOWNLOAD FILE ---
// Download the selected file: send the command and file ID.
void MainWindow::downloadFile() {
    if (address.size() > 0) {
        QByteArray data;
        quint32 size = 18;   // package size including this size uint
        data.append((char*) &size, sizeof(size));
                
        data += "NYANKOUDS";
        quint8 revision = 0;
        data.append((char) revision);        
        data += "FILE";
        
        QTreeWidgetItem* item = remoteList->currentItem();
        QVariant variant = item->data(0, Qt::UserRole);
        quint32 fileID = variant.toUInt();
        data.append((char*) &fileID, sizeof(fileID));
        
        // Ask for the filename to save to.
        QString filepath = QFileDialog::getSaveFileName(this, tr("Save Location."), item->text(0));
        if (filepath.isEmpty()) { return; }
        
        // Set up new download listing in downloads list.
        QStringList row;
        row << item->text(0) << "" << filepath;
        item = new QTreeWidgetItem((QTreeWidgetItem*) 0, row);
        downloadsView->addTopLevelItem(item);
        QProgressBar* progress = new QProgressBar();
        progress->setRange(0, 100);
        downloadsView->setItemWidget(item, 1, progress);
        
        // Send the data array message to a newly created download thread.
        QThread* thread = new QThread;
        DownloadThread* dthread = new DownloadThread(data, filepath, address, 11310);
        dthread->moveToThread(thread);
        connect(dthread, SIGNAL(error(QString)), this, SLOT(errorString(QString)));
        connect(thread, SIGNAL(started()), dthread, SLOT(connectHost()));
        connect(dthread, SIGNAL(finished()), thread, SLOT(quit()));
        connect(dthread, SIGNAL(finished()), dthread, SLOT(deleteLater()));
        connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
        thread->start();
        
        connect(dthread, SIGNAL(progress(int)), progress, SLOT(setValue(int)));
    }
}


// --- GO ONLINE ---
// Change to listen state on server socket.
void MainWindow::goOnline() {
    // do UPnP discovery and report on how many valid IGDs were found.       
    UPNPDev* devlist;
    char lanaddr[64]; // IP address on the LAN
    const char* multicastif = nullptr;
    const char* minissdpdpath = nullptr;
    int error = 0;
    int response;
#if MINIUPNPC_API_VERSION < 14
    /* miniupnpc 1.6 */
    devlist = upnpDiscover(2000, multicastif, minissdpdpath, 0, 0, &error);
#else
    /* miniupnpc 1.9.20150730 */
    devlist = upnpDiscover(2000, multicastif, minissdpdpath, 0, 0, 2, &error);
#endif
    //devlist = upnpDiscover(1000, multicastif, minissdpdpath, UPNP_LOCAL_PORT_ANY, 0, &error);
    if (error > 0) {
        QMessageBox::critical(this, "Error", "UPnP discovery failed: " + QString::number(error));
        return;
    }
    else {
        response = UPNP_GetValidIGD(devlist, &urls, &data, lanaddr, sizeof(lanaddr));
        freeUPNPDevlist(devlist);
        if (response == 0) {
            QMessageBox::critical(this, tr("Devices"), tr("No IGD found."));
        }
        else if (response == 1) {
            //QMessageBox::critical(this, tr("Devices"), tr("A valid connected IGD has been found."));
        }
        else if (response == 2) {
            QMessageBox::critical(this, tr("Devices"), tr("A valid IGD has been found, but it reported as not connected."));
        }
        else if (response == 3) {
            QMessageBox::critical(this, tr("Devices"), tr("An UPnP device was found, but wasn't recognized as an IGD."));
        }
    }
    
    if (response == 1) {
        // obtain external IP address
        int r;
        char externalIPAddress[40];
        UPNPUrls* pUrls = &urls;
        IGDdatas* pData = &data;
        r = UPNP_GetExternalIPAddress(pUrls->controlURL, pData->first.servicetype, externalIPAddress);
        if (r != UPNPCOMMAND_SUCCESS) {
            QMessageBox::critical(this, tr("Error"), tr("GetExternalIPAddress() returned %1").arg( QString::number(r)));
            return;
        }
        
        externalIP = QString::fromLatin1(externalIPAddress, 40);
        
        // connect to IGD and set up port mapping. We use port 11310.
        QList<QNetworkInterface> interfaces = QNetworkInterface::allInterfaces();
        quint32 count = interfaces.size();
        const char* iaddr = nullptr;
        for (quint32 i = 0; i < count; ++i) {
            //QNetworkInterface interface = interfaces[i];
            if (interfaces[i].flags().testFlag(QNetworkInterface::IsUp) &&
                    (!interfaces[i].flags().testFlag(QNetworkInterface::IsLoopBack))) {
                foreach (QNetworkAddressEntry entry, interfaces[i].addressEntries()) {
                    if(entry.ip().protocol() == QAbstractSocket::IPv4Protocol) {
                        //ui->textEdit->append(entry.ip().toString());
                        iaddr = entry.ip().toString().toLatin1().data(); // to const char*
                    }
                }
            }
        }
        
        const char* eport = "11310";                // external port
        const char* iport = "11310";                // internal port
        const char* desc = "Universal Data Share";  // description of port mapping
        const char* proto = "TCP";                  // protocol to use.
        const char* leaseDuration = "0";            // duration of the mapping? Set to 0 as in the example code default.
        r = UPNP_AddPortMapping(pUrls->controlURL, pData->first.servicetype, eport, iport, iaddr, desc, proto, 0, leaseDuration);
        if (r != UPNPCOMMAND_SUCCESS) {
            QMessageBox::critical(this, tr("Error"), tr("AddPortMapping() failed with error: %1").arg(QString::number(r) + " (" + strupnperror(r) + ")"));
        }
    }
    else {
        // FIXME: external IP is local IP?
    }
    
    // set up listening socket on port 11310 using qtcpserver
    tcpserver->listen(QHostAddress::Any, 11310);
    statusView->clearMessage();
    statusView->showMessage(tr("Online"));
    actionGo_offline->setEnabled(true);
    actionGo_online->setEnabled(false);
    online = true;
}


// --- GO OFFLINE ---
// Change to offline, non-listening state.
void MainWindow::goOffline() {
    // shutdown listening socket
    tcpserver->close();
    statusView->clearMessage();
    statusView->showMessage(tr("Offline"));
    actionGo_offline->setEnabled(false);
    actionGo_online->setEnabled(true);
    online = false;
    
    // remove port mapping
    UPNPUrls* pUrls = &urls;
    IGDdatas* pData = &data;
    int r;
    const char* eport = "11310";        // external port
    const char* proto = "TCP";          // protocol to use.
    r = UPNP_DeletePortMapping(pUrls->controlURL, pData->first.servicetype, eport, proto, nullptr);
    if (r != UPNPCOMMAND_SUCCESS) {
        QMessageBox::critical(this, tr("Error"), tr("DeletePortMapping() failed with error: %1").arg(QString::number(r) + " (" + strupnperror(r) + ")"));
        return;
    }
}


// --- LOAD ADDRESS ---
// Attempt to fetch the list of files from the specified address URL.
void MainWindow::loadAddress() {
    address = remoteAddress->text();
    cSocket->connectToHost(address, 11310);
}


// --- SEND DATA ---
// Called after the socket connects to the host.
void MainWindow::sendData() {        
    QByteArray data;
    data.clear();
    quint32 size = 17;   // package size including this size uint
    data.append((char*) &size, sizeof(size));
    
    data += "NYANKOUDS";
    quint8 revision = 0;
    data.append((char) revision);
    
    data += "LIST";
    
    qint64 written = 0;
    while (written != data.size()) {
        written = cSocket->write(data);
        if (written == -1) {
            QMessageBox::critical(this, tr("Error"), tr("Error writing data to client socket: %1").arg(cSocket->errorString()));
            break;
        }
    }    
}


// --- SOCKET ERROR ---
// Called when a socket error occurs.
void MainWindow::socketError(QAbstractSocket::SocketError) {
    if (cSocket->error() == QAbstractSocket::RemoteHostClosedError) { return; }
    QMessageBox::critical(this, tr("Error"), tr("Socket error: %1.").arg(cSocket->errorString()));
    cSocket->close();
}

// --- ERROR STRING ---
// Shows the provided string in an error dialogue.
void MainWindow::errorString(QString err) {
    QMessageBox::critical(this, tr("Error"), err);
}


// --- REFRESH ADDRESS ---
// Attempt to refresh the displayed remote list.
void MainWindow::refreshAddress() {
    if (address.size() > 0) {
        cSocket->connectToHost(address, 11310);
        sendData();
    }
}


// --- RECEIVE DATA ---
// Read data from the client socket.
void MainWindow::receiveData() {    
    QByteArray data = "";
    // read the data length header first. This is an unsigned 32-bit integer.
    // Any additional data read here is a bonus.
    while (data.size() < 4) {
            data += cSocket->readAll();
    }
        
    QByteArray temp = data.mid(0, 4);
    quint32 datasize = *((quint32*) (temp.data()));
    
    qDebug("datasize: " + QString::number(datasize).toLatin1());
    
    // try to read until the full packet size has been read.
    // TODO: off-load this to a separate download thread.
    while (data.size() < datasize) {
        if (cSocket->waitForReadyRead(1000)) {
            data += cSocket->readAll();
        }
        else { break; }
    }
    
    //qDebug("Data size " + QString::number(data.size()).toAscii());
        
    if (data.size() < 1 || data.size() < datasize) {
        QMessageBox::information(this, tr("Read Failure"), tr("Failed to read the response data."));
        return;
    }
    
    // parse the received data.
    //quint32 size = data.mid(0, 4).toInt();
    if (data.mid(4, 6) != "NYANKO") { return; }
    if (data.mid(10, 3) != "UDS") { return; }
    // TODO: check protocol version here
    if (data.mid(14, 4) == "LIST") {
        // Received a LIST response. Process the list.
        //qDebug("Parsing list...");
        remoteList->clear(); // clear the remote list
        int startIndex = 18;
        //int lastIndex = 0;
        quint32 fileID = 0;
        quint64 fileSize = 0;
        quint32 length;
        QTreeWidgetItem* item;
        QByteArray intbytes;
        QByteArray sizebytes;
        //QStringList row;
        QList<QTreeWidgetItem*> items;
        qDebug("Starting loop");
        while (startIndex < data.size()) {
            item = new QTreeWidgetItem();
            intbytes = data.mid(startIndex, 4);
            fileID = *((quint32*) (intbytes.data()));
            qDebug("StartIndex: " + QString::number(startIndex).toLatin1());
            qDebug("FileID: " + QString::number(fileID).toLatin1());
            item->setData(0, Qt::UserRole, QVariant(fileID));
            startIndex += sizeof(fileID);
            sizebytes = data.mid(startIndex, 8);
            fileSize = *((quint64*) (sizebytes.data()));
            item->setText(1, QString::number(fileSize));
            qDebug("StartIndex: " + QString::number(startIndex).toLatin1());
            qDebug("filesize: " + QString::number(fileSize).toLatin1());
            startIndex += sizeof(fileSize);
            intbytes = data.mid(startIndex, 4);
            length = *((quint32*) (intbytes.data()));
            qDebug("StartIndex: " + QString::number(startIndex).toLatin1());
            qDebug("Length: " + QString::number(length).toLatin1());
            startIndex += sizeof(length);
            qDebug("StartIndex: " + QString::number(startIndex).toLatin1());
            item->setText(0, data.mid(startIndex, length));
            //lastIndex += startIndex + length;
            startIndex += length;
            qDebug("StartIndex: " + QString::number(startIndex).toLatin1());
            qDebug("Adding Item to List: " + item->text(0).toLatin1());
            items.append(item);
        }
        
        remoteList->addTopLevelItems(items);
    }
    else {
        QMessageBox::information(this, tr("Unknown Response"), tr("Received unknown response."));
    }
    
    cSocket->close();
}



// --- INCOMING CONNECTION ---
// Called when there's a new client connection pending.
void MainWindow::incomingConnection() {    
    // obtain the client socket
    sSocket = tcpserver->nextPendingConnection();
    //sSockets.append(sSocket);
    connect(sSocket, SIGNAL(disconnected()), sSocket, SLOT(deleteLater()));
    connect(sSocket, SIGNAL(readyRead()), this, SLOT(serverRead()));
}


// --- SERVER READ ---
void MainWindow::serverRead() {
    qDebug("Server read...");
    QByteArray data;
    while (data.size() < 4) {
        data += sSocket->readAll();
    }
    
    QByteArray size = data.mid(0, 4);
    quint32 datasize = *(quint32*)(size.data());
    
    qDebug("datasize: " + QString::number(datasize).toLatin1());
    qDebug("Data size " + QString::number(data.size()).toLatin1());
    qDebug("sSocket size: " + QString::number(sSocket->bytesAvailable()).toLatin1());
    
    // try to read until the full packet size has been read.
    while (data.size() < datasize) {
        if (sSocket->waitForReadyRead(1000)) {
            data += sSocket->readAll();
        }
        else { break; }
    }
    
    qDebug("Data size " + QString::number(data.size()).toLatin1());
        
    if (data.size() < 1 || data.size() < datasize) {
        sSocket->disconnectFromHost();
        return;
    }
    
    // parse the received data. If correct, return the file list.
    if (data.mid(4, 6) != "NYANKO") { sSocket->disconnectFromHost(); return; }
    if (data.mid(10, 3) != "UDS") { sSocket->disconnectFromHost(); return; }
    // TODO: check for protocol version here
    if (data.mid(14, 4) == "LIST") {
        // LIST command
        // Return the list of available files.
        QTreeWidgetItem* item;
        quint32 length;
        quint64 filesize;
        QByteArray output;
        QByteArray filename;
        output = "NYANKOUDS";
        quint8 revision = 0;
        output.append((char) revision);        
        output += "LIST";
        for (quint32 i = 0; i < localList->topLevelItemCount(); ++i) {
            item = localList->topLevelItem(i);
            filename = item->text(0).toLatin1();
            length = filename.size();
            output.append((char*) &i, sizeof(i)); // file ID            
            QFileInfo info = files[i]->fileName();
            filesize = (quint64) info.size();
            
            output.append((char*) &filesize, sizeof(filesize)); // file size
            output.append((char*) &length, sizeof(length)); // file name length            
            output.append(filename);   // file name
        }
        
        QByteArray lba;
        length = output.size() + sizeof(length);
        lba.append((char*) &length, sizeof(length));        
        output.prepend(lba); // data length prefix
        
        // write to client socket
        qint64 written = 0;
        while (written != output.size()) {
            written = sSocket->write(output);
            if (written == -1) {
                QMessageBox::critical(this, tr("Error"), tr("Error writing data to server socket: %1").arg(cSocket->errorString()));
                break;
            }
        }
    }
    else if (data.mid(14, 4) == "FILE") {
        // FILE command        
        // Return the specified file.
        QByteArray intbytes;
        intbytes = data.mid(18, 4);
        quint32 id = *(quint32*) (intbytes.data());        
        if (id > files.size() - 1) { sSocket->disconnectFromHost(); return; }
                
        // start the upload thread.
        QThread* thread = new QThread;
        UploadThread* dthread = new UploadThread(files[id], sSocket);
        dthread->moveToThread(thread);
        connect(dthread, SIGNAL(error(QString)), this, SLOT(errorString(QString)));
        connect(thread, SIGNAL(started()), dthread, SLOT(sendData()));
        connect(dthread, SIGNAL(finished()), thread, SLOT(quit()));
        connect(dthread, SIGNAL(finished()), dthread, SLOT(deleteLater()));
        connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
        thread->start();
    }
    else { sSocket->disconnectFromHost(); return; }
}


// --- ABOUT ---
// Shows the About dialogue.
void MainWindow::about() {
    QMessageBox::about(this, tr("About"), tr("Universal Data Share 0.1-alpha - by Maya Posch.\nwww.mayaposch.com"));
}


// --- EXIT ---
// Exits the application, cleaning up any resources used.
void MainWindow::quit() {
    exit(0);
}
