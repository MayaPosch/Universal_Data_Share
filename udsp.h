/*
    udsp.h - declarations for Universal Data Share protocol.
    
    Revision 0
    
    Notes:
            - 
            
    2011/10/26, Maya Posch
    (c) Nyanko.ws
*/

#ifndef UDSP_H
#define UDSP_H

#include <QByteArray>


// --- ENUMS ---
enum {
    UDS_CMD_LIST = 0,
    UDS_CMD_FILE = 1
};


// --- STRUCTS ---
struct UdsResponse {
    //
};

struct UdsRequest {
    quint32 id;
};


// --- FUNCTIONS ---
int UdsComposeRequest(QByteArray &out, quint8 command, UdsRequest &data);
int UdsComposeResponse(QByteArray &out, quint8 command, UdsResponse &data);
UdsResponse UdsParseResponse();


#endif // UDSP_H
