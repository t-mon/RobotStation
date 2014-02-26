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
    m_capture.open(m_camera);
    if(!m_capture.isOpened()){
        qDebug() << "ERROR: could not open camera.";
        return;
    }
    qDebug() << "Camera open.";
    m_timer->start();
}

void CameraEngine::stopEngine()
{
    m_timer->stop();

    if(!m_capture.isOpened()){
        m_capture.release();
        qDebug() << "Camera engine stopped.";
    }
}


