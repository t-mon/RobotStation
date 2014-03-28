#include "core.h"

Core *Core::s_instance = NULL;

Core::Core(QObject *parent) :
    QObject(parent)
{
}

void Core::init()
{
    m_window = new MainWindow(0);
    m_cameraEngine = new CameraEngine(m_window);

    m_imageProcessor = new ImageProcessor(m_window);

    connect(m_cameraEngine,SIGNAL(imageReady(Mat)),m_imageProcessor,SLOT(updateImage(Mat)));
    connect(m_window,SIGNAL(fpsChanged(int)),m_imageProcessor,SLOT(setFps(int)));
    connect(m_window,SIGNAL(processTypeChanged(int)),m_imageProcessor,SLOT(processTypeChanged(int)));
    connect(m_window,SIGNAL(thresholdValueChanged(int)),m_imageProcessor,SLOT(thresholdValueChanged(int)));
    connect(m_window,SIGNAL(brightnessValueChanged(double)),m_imageProcessor,SLOT(brightnessValueChanged(double)));
    connect(m_window,SIGNAL(contrastValueChanged(double)),m_imageProcessor,SLOT(contrastValueChanged(double)));
    connect(m_window,SIGNAL(startVideo()),m_cameraEngine,SLOT(startEngine()));
    connect(m_window,SIGNAL(stopVideo()),m_cameraEngine,SLOT(stopEngine()));

    m_window->show();
    m_cameraEngine->startEngine();

}

ImageProcessor *Core::imageProcessor()
{
    return m_imageProcessor;
}

MainWindow *Core::window()
{
    return m_window;
}
