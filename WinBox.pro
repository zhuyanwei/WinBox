#-------------------------------------------------
#
# Project created by QtCreator 2016-12-28T19:39:44
#
#-------------------------------------------------

QT       += core gui

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

INCLUDEPATH += D:\OpenCV\install\include \
D:\ffmpeg322\include \
D:\RTP\jrtpnothread\include \
D:\PA\PAnoASIO\include

LIBS += -LD:\OpenCV\install\bin -llibopencv_core231 -llibopencv_highgui231 \
-LD:\ffmpeg322\lib -llibavformat -llibavdevice -llibavcodec -llibavutil -llibswscale -llibswresample \
-LD:\RTP\jrtpnothread\lib -llibjrtplib_d \
-LD:\PA\PAnoASIO\lib -llibportaudio \
-LD:\Qt\Qt5.7.0\5.7\mingw53_32\lib -llibQt5Network \
-lws2_32



