#-------------------------------------------------
#
# Project created by QtCreator 2014-02-26T18:15:58
#
#-------------------------------------------------

QT       += core gui widgets

TARGET = RobotStation
TEMPLATE = app

#include opencv
LIBS += -lopencv_core\
        -lopencv_highgui\
        -lopencv_imgproc\
        -lopencv_video\
        -lopencv_calib3d\
        -lzbar\


SOURCES += main.cpp\
        mainwindow.cpp \
        core.cpp \
        cameraengine.cpp \
        snapshotdialog.cpp \
        calibrationdialog.cpp \
        markergeneratordialog.cpp \
        markersearchengine.cpp \
        imageprocessor.cpp

HEADERS  += mainwindow.h \
        core.h \
        cameraengine.h \
        snapshotdialog.h \
        calibrationdialog.h \
        markergeneratordialog.h \
        markersearchengine.h \
        imageprocessor.h
