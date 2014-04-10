#include "core.h"

Core *Core::s_instance = NULL;

Core::Core(QObject *parent) :
    QObject(parent)
{
}

void Core::init()
{
    m_window = new MainWindow(0);
    m_cameraEngine = new CameraEngine(0);
    m_imageProcessor = new ImageProcessor(m_window);
    m_robot = new RobotCommander(m_window);

    m_window->show();

    //m_cameraEngine->startEngine();
}

CameraEngine *Core::cameraEngine()
{
    return m_cameraEngine;
}

ImageProcessor *Core::imageProcessor()
{
    return m_imageProcessor;
}

MainWindow *Core::window()
{
    return m_window;
}

RobotCommander *Core::robot()
{
    return m_robot;
}
