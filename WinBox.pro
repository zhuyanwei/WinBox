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
    DecodeAU.cpp  \
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

INCLUDEPATH += /usr/include \
/usr/local/JRtp/include

LIBS += -L/usr/local/FFMpeg/lib/ -lavformat -lavdevice -lavcodec -lavutil -lswscale -lswresample \
-L/usr/local/JRtp/lib -ljrtp \
-L/usr/lib/arm-linux-gnueabihf/ -lopencv_core -lopencv_highgui \
-L/usr/lib/arm-linux-gnueabihf/ -lportaudio



