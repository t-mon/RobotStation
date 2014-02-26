#ifndef CAMERAENGINE_H
#define CAMERAENGINE_H

#include <QThread>
#include <QTimer>
#include <QDebug>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;

class CameraEngine : public QObject
{
    Q_OBJECT
public:
    explicit CameraEngine(QObject *parent = 0);

private:
    VideoCapture m_capture;
    QTimer *m_timer;
    int m_camera;

signals:
    void imageReady(const Mat &image);

private slots:
    void captureImage();

public slots:
    void startEngine();
    void stopEngine();


};

#endif // CAMERAENGINE_H
