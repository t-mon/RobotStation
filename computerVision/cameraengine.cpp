/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  Copyright 2014 Simon Stuerz                                            *
 *  This file is part of RobotStation.                                     *
 *                                                                         *
 *  RobotStation is free software: you can redistribute it and/or modify   *
 *  it under the terms of the GNU General Public License as published by   *
 *  the Free Software Foundation, version 2 of the License.                *
 *                                                                         *
 *  RobotStation is distributed in the hope that it will be useful         *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the           *
 *  GNU General Public License for more details.                           *
 *                                                                         *
 *  You should have received a copy of the GNU General Public License      *
 *  along with RobotStation. If not, see <http://www.gnu.org/licenses/>.   *
 *                                                                         *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "cameraengine.h"
#include "core.h"

CameraEngine::CameraEngine(QObject *parent) :
    QThread(parent)
{   
    QSettings settings("RobotStation");
    m_camera = settings.value("camera",0).toInt();
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
        qDebug() << "ERROR: could not open camera capture" << m_camera;
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

        // Capture frame (if available)
        if (!capture.grab())
            continue;

        if(!capture.retrieve(image)){
            qDebug() << "ERROR: could not read image image from camera.";
        }
        emit updateImage(image);
    }
    qDebug() << "camera thread stopped";
    capture.release();
    if(capture.isOpened()){
        qDebug() << "could not release capture";
    }
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

void CameraEngine::updateCamera(const int &camera)
{
    stopEngine();
    wait(1000);
    QSettings settings("RobotStation");
    settings.setValue("camera",camera);
    m_camera = camera;
    qDebug() << "camera changed to " << camera;
    startEngine();
}


