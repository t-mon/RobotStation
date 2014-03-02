#ifndef IMAGEPROCESSOR_H
#define IMAGEPROCESSOR_H

#include <QObject>
#include <QImage>
#include <QTimer>
#include <QDebug>
#include <QList>
#include <QSettings>
#include <QDir>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <markersearchengine.h>

using namespace cv;

class ImageProcessor : public QObject
{
    Q_OBJECT
public:
    explicit ImageProcessor(QObject *parent = 0);
    Mat convertQImageToMat(QImage image);
    QImage convertMatToQimage(Mat imageMat);
    void saveCalibrationParameter(Mat intrinsic, Mat extrinsic);

private:
    QTimer *m_timer;
    int m_fps;

    Mat m_image;

    Mat m_intrinsic;
    Mat m_extrinsic;
    bool m_calibrated;

    int m_processType;
    int m_threshold;

    double m_alpha;
    double m_beta;

    MarkerSearchEngine *m_markerSearchEngine;

    void loadSettings();

private slots:
    void processImage();

signals:
    void imageReady(const Mat &image);
    void originalImageReady(const Mat &image);

public slots:
    void updateImage(const Mat &image);
    void processTypeChanged(const int& processType);
    void thresholdValueChanged(const int& threshold);
    void brightnessValueChanged(const double &brightness);
    void contrastValueChanged(const double &contrast);

    void setFps(const int &fps);
    void startProcessor();
    void stopProcessor();

};

#endif // IMAGEPROCESSOR_H
