#ifndef CORE_H
#define CORE_H

#include <QObject>

#include "gui/mainwindow.h"
#include "computerVision/cameraengine.h"
#include "computerVision/imageprocessor.h"
#include "robotCommunication/robotcommander.h"

class Core : public QObject
{
    Q_OBJECT
public:
    static Core *instance() {
        if(!s_instance) {
            s_instance = new Core();
            s_instance->init();
        }
        return s_instance;
    }

    CameraEngine *cameraEngine();
    ImageProcessor *imageProcessor();
    MainWindow *window();
    RobotCommander *robot();

private:
    explicit Core(QObject *parent = 0);
    static Core *s_instance;

    CameraEngine *m_cameraEngine;
    ImageProcessor *m_imageProcessor;
    MainWindow *m_window;
    RobotCommander *m_robot;


private slots:
    void init();

signals:

public slots:

};

#endif // CORE_H
