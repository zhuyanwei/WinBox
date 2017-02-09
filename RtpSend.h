#ifndef RTPSEND_H
#define RTPSEND_H

#include <QObject>
#include <QDebug>
#include <COMDEF.h>
#include "COMRTP.h"

using namespace jrtplib;

typedef struct
{
    // Little Endian
    // byte 0
    unsigned char csrc_len :4;    // contains the number of CSRC identifiers that follow the fixed header.
    unsigned char extension :1;    // extension bit
    unsigned char padding :1;    // padding
    unsigned char version :2;    // version, current is 2
    // byte 1
    unsigned char payload :7;    // payload
    unsigned char marker :1;    // marker
    // bytes 2, 3
    unsigned short seq_no;
    // bytes 4-7
    unsigned long timestamp;
    // bytes 8-11
    unsigned long ssrc;    // sequence number
} rtp_header;

typedef struct
{
    unsigned char TYPE :5;
    unsigned char NRI :2;
    unsigned char F :1;
} nalu_header;    // 1 Bytes

typedef struct
{
    //byte 0
    unsigned char TYPE :5;
    unsigned char NRI :2;
    unsigned char F :1;
} fu_indicator;    // 1 BYTE

typedef struct
{
    //byte 0
    unsigned char TYPE :5;
    unsigned char R :1;
    unsigned char E :1;
    unsigned char S :1;
} fu_header;    // 1 BYTE

typedef struct
{
    int startcodeprefix_len;    // 0x000001 or 0x00000001
    char *data;    // nalu data
    int len;    // nalu length
    int forbidden_bit;    //
    int nal_reference_idc;    //
    int nal_unit_type;    // nalu types
} nalu_t;

class RtpSend : public QObject
{
    Q_OBJECT
public:
    RtpSend();
    ~RtpSend();
    int netOpen(RTPSession *sess);
    void netClose();
    int packOpen(int maxPktLen,int ssrc);
    void packPut(void *inBuf,int inSize);
    int packGet(void *outBuf, int bufSize,int *outSize);

    RTPSession *session;
    RTPSessionParams sessionParams;
    RTPUDPv4TransmissionParams transParams;
    int status;
    nalu_header		*nalu_hdr;
    fu_indicator	*fu_ind;
    fu_header		*fu_hdr;
    char sendBuf[MAXDATASIZE];
    char* naluPayload;
    unsigned int timestampIncrese,tsCurrent;
    nalu_t *n;

    void *inBuf;
    char *nextNaluPtr;
    int inBufSize;
    int FUCounter;
    int lastFUSize;
    int FUIndex;
    int inBufComplete;
    int naluComplete;
    nalu_t nalu;
    unsigned short seqNum;
    U32 tsStartMillisec;
    U32 tsCurrentSample;
    int maxPktLen;
    int ssrc;

private:
    U32 getCurrentMillisec(void);
    int isStartCode4(char *buf);
    int isStartCode3(char *buf);
    int getNextNalu();
    void dumpNalu(const nalu_t *nalu);

signals:

public slots:
};

#endif // RTPSEND_H
