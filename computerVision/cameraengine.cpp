#include "cameraengine.h"
#include "core.h"

CameraEngine::CameraEngine(QObject *parent) :
    QThread(parent)
{   
    m_camera = 0;
    m_stop = false;
    connect(this,SIGNAL(imageReady(Mat)),this,SLOT(updateImage(Mat)));
    connect(this,SIGNAL(fpsReady(int)),this,SLOT(updateFps(double)));
}

Mat CameraEngine::image()
{
    return m_image;
}

void CameraEngine::run()
{
    qDebug() << "camera thread started";
    VideoCapture capture;
    capture.open(m_camera);
    if(!capture.isOpened()){
        qDebug() << "ERROR: could not open camera capture";
    }

    while(1){
        // check if we should stop the thread...
        m_stopMutex.lock();
        if(m_stop){
            m_stop=false;
            m_stopMutex.unlock();
            break;
        }
        m_stopMutex.unlock();

        int captureTime = m_time.elapsed();
        updateFps(captureTime);
        m_time.start();

        // Capture frame (if available)
        if (!capture.grab())
            continue;

        Mat image;
        if(!capture.retrieve(image)){
            qDebug() << "ERROR: could not read image image from camera.";
        }
        emit updateImage(image);
        emit fpsReady(captureTime);
    }
    qDebug() << "camera thread stopped";
    capture.release();
    if(capture.isOpened()){
        qDebug() << "could not release capture";
    }
}

void CameraEngine::updateFps(double fps)
{
    float dt = fps - m_captureTime;
    m_captureTime = fps;
    float actualFps = (float)1000/dt;
    Core::instance()->window()->setStatusBarText("fps: " + QString::number(actualFps));
}

void CameraEngine::updateImage(const Mat &image)
{
    m_image = image.clone();
}

void CameraEngine::startEngine()
{
    m_stopMutex.lock();
    m_stop = false;
    m_stopMutex.unlock();

    start();
    Core::instance()->imageProcessor()->startProcessor();
}

void CameraEngine::stopEngine()
{
    m_stopMutex.lock();
    m_stop = true;
    m_stopMutex.unlock();

    Core::instance()->imageProcessor()->stopProcessor();
}


