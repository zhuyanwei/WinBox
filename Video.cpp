#include "Video.h"

Video::Video()
{
    deR = new Decode();
    ret=deR->decodeOpen();
    ret=deR->convertOpen2();
}

Video::~Video()
{

}

void Video::readingFrame(void *buf,int len)
{
    qDebug()<<"readingFrame start";
    deR->decodeDo(buf,len,&disBufR);
    emit decodeDone();
}

//void Video::readingFrame()
//{
//    rtpsess->BeginDataAccess();
//    if( rtpsess->GotoFirstSourceWithData() )
//    {
//        do
//        {
//            while( ( rtppack = rtpsess->GetNextPacket() ) != NULL )
//            {
//                if (rtppack->GetPayloadType() == AAC)
//                {
//                   receive_bytes = rtppack->GetPayloadLength();
//                   recv_bufa = (char *)rtppack->GetPayloadData();
//                   ad->decode_do(recv_bufa,receive_bytes);
//                }
//                else if (rtppack->GetPayloadType() == H264)
//                {
//                    uint32_t packssrc = rtppack->GetSSRC();
//                    int number=0;
//                    if (packssrc == ssrc[0]) number = 1;
//                    else if(packssrc == ssrc[1]) number = 2;
//                    switch (number)
//                    {
//                        case 1:
//                        {
//                            receive_bytes = rtppack->GetPayloadLength();
//                            totalsize1 += receive_bytes;
//                            recv_buf1 = (char *)rtppack->GetPayloadData();
//                            marker = rtppack->HasMarker();
//                            if(rr1->rtp_unpackage(recv_buf1,receive_bytes,marker,&de_buf1,&de_len) == 1)
//                            {
//                                if (de1->decode_do(de_buf1,de_len,&dis_buf1) ==1)
//                                {
//                                    update1 = true;
//                                    emit getframe();
//                                }
//                            }
//                            break;
//                        }
//                        case 2:
//                        {
//                            receive_bytes = rtppack->GetPayloadLength();
//                            totalsize2 += receive_bytes;
//                            recv_buf2 = (char *)rtppack->GetPayloadData();
//                            marker = rtppack->HasMarker();
//                            if(rr2->rtp_unpackage(recv_buf2,receive_bytes,marker,&de_buf2,&de_len) == 1)
//                            {
//                                if (de2->decode_do(de_buf2,de_len,&dis_buf2) ==1)
//                                {
//                                    update2 = true;
//                                    emit getframe();
//                                }
//                            }
//                            break;
//                        }
//                    }
//                }
//            rtpsess->DeletePacket( rtppack );
//            }
//        }
//        while( rtpsess->GotoNextSourceWithData() );
//    }
//    rtpsess->EndDataAccess();
//}

void Video::showRemoteWindow()
{

}
