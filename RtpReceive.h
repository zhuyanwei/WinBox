#ifndef RTPRECEIVE_H
#define RTPRECEIVE_H

#include <QObject>
#include <QDebug>
#include "COMDEF.h"
#include "COMRTP.h"

using namespace jrtplib;
using namespace std;

typedef struct
{
    unsigned char forbidden_bit;
    unsigned char nal_reference_idc;
    unsigned char nal_unit_type;
    unsigned int startcodeprefix_len;
    unsigned int len;
    unsigned int max_size;
    unsigned char * buf;
    unsigned int lost_packets;
} NALU_t;

typedef struct
{
    //byte 0
    unsigned char TYPE:5;
    unsigned char NRI:2;
    unsigned char F:1;
} NALU_HEADER; // 1 BYTE

typedef struct
{
    //byte 0
    unsigned char TYPE:5;
    unsigned char NRI:2;
    unsigned char F:1;
} FU_INDICATOR; // 1 BYTE

typedef struct
{
    //byte 0
    unsigned char TYPE:5;
    unsigned char R:1;
    unsigned char E:1;
    unsigned char S:1;
} FU_HEADER;   // 1 BYTES

class RtpReceive : public QObject
{
    Q_OBJECT
public:
    RtpReceive();
    ~RtpReceive();
    void freeNALU(NALU_t *n);
    int initiateWinsock(RTPSession *sess);
    int rtpUnpackage(char *bufIn, int len, bool marker, void **bufOut, int *olen);
    void netClose();
    void checkError( int errCode );

    RTPSession *rtpSess;
    RTPSessionParams sessionPara;
    RTPUDPv4TransmissionParams transPara;
    unsigned short localPort;
    int status, timeout;
    RTPPacket *rtpPack;
    QUdpSocket *udpSocket;
    int port;

signals:

public slots:
};

#endif // RTPRECEIVE_H
