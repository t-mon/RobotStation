#include "cameraengine.h"
#include "core.h"

CameraEngine::CameraEngine(QObject *parent) :
    QObject(parent)
{   
    m_camera = CV_CAP_ANY;

    m_timer = new QTimer(this);
    m_timer->setInterval(10);

    connect(m_timer,SIGNAL(timeout()),this,SLOT(captureImage()));
}

void CameraEngine::captureImage()
{
    Mat image;
    if(m_capture.read(image)){
        emit imageReady(image);
    }else{
        qDebug() << "ERROR: could not read image from camera.";
    }
}

void CameraEngine::startEngine()
{
    if(m_capture.isOpened()){
        return;
    }

    m_capture.open(m_camera);

    if(!m_capture.isOpened()){
        qDebug() << "ERROR: could not open camera.";
        return;
    }
    qDebug() << "Camera open.";
    m_timer->start();
    Core::instance()->imageProcessor()->startProcessor();
}

void CameraEngine::stopEngine()
{
    m_timer->stop();
    Core::instance()->imageProcessor()->stopProcessor();

    if(m_capture.isOpened()){
        m_capture.release();
        qDebug() << "Camera engine stopped.";
    }
}


