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

#ifndef CAMERAENGINE_H
#define CAMERAENGINE_H

#include <QThread>
#include <QMutex>
#include <QTime>
#include <QDebug>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;

class CameraEngine : public QThread
{
    Q_OBJECT
public:
    explicit CameraEngine(QObject *parent = 0);
    Mat image();
    Size frameSize();
private:
    int m_camera;
    Mat m_image;
    Size m_frameSize;
    QMutex m_stopMutex;
    bool m_stop;

    QMutex m_imageMutex;

protected:
    void run();

signals:
    void imageReady(const Mat &image);
    void fpsReady(const int &fps);

private slots:
    void updateImage(const Mat &image);

public slots:
    void startEngine();
    void stopEngine();
    void updateCamera(const int &camera);

};

#endif // CAMERAENGINE_H
