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

using namespace cv;

class ImageProcessor : public QObject
{
    Q_OBJECT
public:
    explicit ImageProcessor(QObject *parent = 0);
    Mat convertQImageToMat(QImage image);
    QImage convertMatToQimage(Mat imageMat);
    void saveCalibrationParameter(Mat intrinsic, Mat extrinsic);

    enum ProcessType{
        OriginalImage = 0,
        UndistortImage = 1,
        GrayImage = 2,
        GrayEqualizedImage = 3,
        GaussianBlurImage = 4,
        ThresholdImage = 5,
        AdaptiveThresholdImage = 6,
        CannyImage = 7,
        ResultImage = 8
    };


private:
    QTimer *m_timer;
    int m_fps;

    Mat m_image;

    Mat m_intrinsic;
    Mat m_extrinsic;
    bool m_calibrated;

    ProcessType m_processType;

    void loadSettings();

private slots:
    void processImage();

signals:
    void imageReady(const Mat &image);

public slots:
    void updateImage(const Mat &image);
    void processTypeChanged(const int& processType);
    void setFps(const int &fps);

};

#endif // IMAGEPROCESSOR_H
