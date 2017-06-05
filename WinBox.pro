#-------------------------------------------------
#
# Project created by QtCreator 2016-12-28T19:39:44
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = WinBox
TEMPLATE = app


SOURCES += main.cpp\
        widget.cpp \
    CameraGet.cpp \
    Encode.cpp \
    Decode.cpp \
    ThreadCamera.cpp \
    Convert.cpp \
    RtpSend.cpp \
    RtpReceive.cpp \
    MicGet.cpp \
    ThreadMic.cpp \
    EncodeAU.cpp \
    DecodeAU.cpp \
    Show.cpp

HEADERS  += widget.h \
    CameraGet.h \
    Encode.h \
    Decode.h \
    ThreadCamera.h \
    COMDEF.h \
    FFMPEG_COMMON.h \
    Convert.h \
    RtpSend.h \
    COMRTP.h \
    RtpReceive.h \
    MicGet.h \
    ThreadMic.h \
    EncodeAU.h \
    DecodeAU.h \
    Show.h

FORMS    += widget.ui \
    Show.ui

INCLUDEPATH += E:\OpenCV\install\include \
E:\ffmpeg322\include \
E:\RTP\jrtpnothread\include \
E:\PA\PAnoASIO\include

LIBS += -LE:\OpenCV\install\bin -llibopencv_core231 -llibopencv_highgui231 \
-LE:\ffmpeg322\lib -llibavformat -llibavdevice -llibavcodec -llibavutil -llibswscale -llibswresample \
-LE:\RTP\jrtpnothread\lib -llibjrtplib_d \
-LE:\PA\PAnoASIO\lib -llibportaudio \
-lws2_32



