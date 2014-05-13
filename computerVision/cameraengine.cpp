#include "cameraengine.h"
#include "core.h"

CameraEngine::CameraEngine(QObject *parent) :
    QThread(parent)
{   
    m_camera = 1;
    m_stop = false;
    connect(this,SIGNAL(imageReady(Mat)),this,SLOT(updateImage(Mat)));
}

Mat CameraEngine::image()
{
    m_imageMutex.lock();
    Mat image = m_image;
    m_imageMutex.unlock();
    return image;
}

Size CameraEngine::frameSize()
{
    return m_frameSize;
}

void CameraEngine::run()
{
    qDebug() << "camera thread started";
    VideoCapture capture;
    capture.open(m_camera);
    if(!capture.isOpened()){
        qDebug() << "ERROR: could not open camera capture";
    }
    qDebug() << "   Frame Size: " << capture.get(CV_CAP_PROP_FRAME_WIDTH) << capture.get(CV_CAP_PROP_FRAME_HEIGHT);
    m_frameSize = Size(capture.get(CV_CAP_PROP_FRAME_WIDTH), capture.get(CV_CAP_PROP_FRAME_HEIGHT));

    Mat image;
    while(1){
        // check if we should stop the thread...
        m_stopMutex.lock();
        if(m_stop){
            m_stop=false;
            m_stopMutex.unlock();
            break;
        }
        m_stopMutex.unlock();

        m_startTime = m_time.elapsed();
        m_time.start();


        // Capture frame (if available)
        if (!capture.grab())
            continue;

        //qDebug() << "fps:" << fps;

        if(!capture.retrieve(image)){
            qDebug() << "ERROR: could not read image image from camera.";
        }
        emit updateImage(image);
        //emit fpsReady(captureTime);
    }
    qDebug() << "camera thread stopped";
    capture.release();
    if(capture.isOpened()){
        qDebug() << "could not release capture";
    }
}

void CameraEngine::updateFps(double fps)
{
    //float dt = fps - m_captureTime;
    //m_captureTime = fps;
    //float actualFps = (float)1000/dt;
    //Core::instance()->window()->setStatusBarText("fps: " + QString::number(actualFps));
}

void CameraEngine::updateImage(const Mat &image)
{
    m_imageMutex.lock();
    m_image = image;
    m_imageMutex.unlock();
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


