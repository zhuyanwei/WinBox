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
    ThreadCamera.cpp

HEADERS  += widget.h \
    CameraGet.h \
    Encode.h \
    Decode.h \
    ThreadCamera.h \
    COMDEF.h \
    FFMPEG_COMMON.h

FORMS    += widget.ui

INCLUDEPATH += D:\OpenCV\install\include\opencv \
D:\OpenCV\install\include \
D:\ffmpeg322\include

LIBS += -LD:\OpenCV\install\bin -llibopencv_core231 -llibopencv_highgui231 \
D:\ffmpeg322\lib\libavformat.dll.a \
D:\ffmpeg322\lib\libavdevice.dll.a \
D:\ffmpeg322\lib\libavcodec.dll.a \
D:\ffmpeg322\lib\libavutil.dll.a \
D:\ffmpeg322\lib\libswscale.dll.a \
D:\ffmpeg322\lib\libswresample.dll.a

