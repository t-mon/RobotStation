#ifndef CORE_H
#define CORE_H

#include <QObject>

#include "cameraengine.h"
#include "mainwindow.h"
#include "imageprocessor.h"

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

    ImageProcessor *imageProcessor();
    MainWindow *window();

private:
    explicit Core(QObject *parent = 0);
    static Core *s_instance;

    CameraEngine *m_cameraEngine;
    ImageProcessor *m_imageProcessor;
    MainWindow *m_window;


private slots:
    void init();

signals:

public slots:

};

#endif // CORE_H
