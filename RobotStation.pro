#-------------------------------------------------
#
# Project created by QtCreator 2014-02-26T18:15:58
#
#-------------------------------------------------

#include qt moules
QT       += core gui widgets network

TARGET = RobotStation
TEMPLATE = app


#include opencv and zbar libs
LIBS += -lopencv_core\
        -lopencv_highgui\
        -lopencv_imgproc\
        -lopencv_video\
        -lopencv_calib3d\
        -lzbar\


SOURCES += main.cpp\
        gui/mainwindow.cpp \
        core.cpp \
        computerVision/cameraengine.cpp \
        tools/snapshotdialog.cpp \
        tools/calibrationdialog.cpp \
        tools/markergeneratordialog.cpp \
        computerVision/markersearchengine.cpp \
        computerVision/imageprocessor.cpp \
        tools/qrcodesearchdialog.cpp \
        computerVision/marker.cpp \
        tools/markersearchdialog.cpp \
        computerVision/poseestimationengine.cpp \
        robotCommunication/robottcpclient.cpp \
        robotCommunication/robotcommander.cpp \
        gui/settingsdialog.cpp \
        gui/robotcontrolwidget.cpp \
        gui/cameracontrolwidget.cpp \
    robotCommunication/robottcpserver.cpp

HEADERS  += gui/mainwindow.h \
        core.h \
        computerVision/cameraengine.h \
        tools/snapshotdialog.h \
        tools/calibrationdialog.h \
        tools/markergeneratordialog.h \
        computerVision/markersearchengine.h \
        computerVision/imageprocessor.h \
        tools/qrcodesearchdialog.h \
        computerVision/marker.h \
        tools/markersearchdialog.h \
        computerVision/poseestimationengine.h \
        robotCommunication/robottcpclient.h \
        robotCommunication/robotcommander.h \
        gui/settingsdialog.h \
        gui/robotcontrolwidget.h \
        gui/cameracontrolwidget.h \
    robotCommunication/robottcpserver.h

OTHER_FILES +=

RESOURCES += \
    icons/icons.qrc
