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
private:
    int m_camera;
    Mat m_image;
    QMutex m_stopMutex;
    bool m_stop;

    // fps calculation
    QTime m_time;
    int m_fpsSum;
    int m_captureTime;
    int m_sampleNumber;

protected:
    void run();

signals:
    void imageReady(const Mat &image);
    void fpsReady(const int &fps);

private slots:
    void updateFps(double fps);
    void updateImage(const Mat &image);

public slots:
    void startEngine();
    void stopEngine();


};

#endif // CAMERAENGINE_H