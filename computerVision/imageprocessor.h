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

    PoseEstimationEngine *m_poseEngine;

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

    void updateFps(const int &fps);
    void startProcessor();
    void stopProcessor();

};

#endif // IMAGEPROCESSOR_H
