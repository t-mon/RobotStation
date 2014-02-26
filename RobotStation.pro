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
        Tools/snapshotdialog.cpp \
        Tools/calibrationdialog.cpp \
        Tools/markergeneratordialog.cpp \
        markersearchengine.cpp \
        imageprocessor.cpp \
        Tools/thresholddialog.cpp \
        Tools/qrcodesearchdialog.cpp

HEADERS  += mainwindow.h \
        core.h \
        cameraengine.h \
        Tools/snapshotdialog.h \
        Tools/calibrationdialog.h \
        Tools/markergeneratordialog.h \
        markersearchengine.h \
        imageprocessor.h \
        Tools/thresholddialog.h \
        Tools/qrcodesearchdialog.h
