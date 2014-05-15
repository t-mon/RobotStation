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

#ifndef IMAGEPROCESSOR_H
#define IMAGEPROCESSOR_H

#include <QObject>
#include <QImage>
#include <QTimer>
#include <QDebug>
#include <QList>
#include <QSettings>
#include <QDir>
#include <QMutex>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "poseestimationengine.h"

using namespace cv;

class ImageProcessor : public QObject
{
    Q_OBJECT
public:
    explicit ImageProcessor(QObject *parent = 0);

    Mat convertQImageToMat(QImage image);
    QImage convertMatToQimage(Mat imageMat);

    void saveCalibrationParameter(Mat intrinsic, Mat extrinsic);
    Mat getIntrinsic();
    Mat getExtrinsic();
    Point2f getOpticalCenter();
    QImage image();

private:
    QTimer *m_timer;
    int m_fps;

    QMutex m_imageMutex;
    Mat m_image;
    Mat m_processedImage;

    Mat m_intrinsic;
    Mat m_extrinsic;
    bool m_calibrated;

    int m_processType;
    double m_alpha;
    double m_beta;

    bool m_flipHorizontal;
    bool m_flipVertical;

    void loadSettings();

private slots:
    void processImage();
    void updateImage();

signals:
    void imageReady(const Mat &image);
    void originalImageReady(const Mat &image);

public slots:
    void updateProcessType(const int& processType);
    void updateBrightness(const double &brightness);
    void updateContrast(const double &contrast);
    void updateHorizontalFlip(const bool &flip);
    void updateVerticalFlip(const bool &flip);

    void updateFps(const int &fps);
    void startProcessor();
    void stopProcessor();

};

#endif // IMAGEPROCESSOR_H
