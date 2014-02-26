#ifndef MARKERSEARCHENGINE_H
#define MARKERSEARCHENGINE_H

#include <QObject>
#include <QList>
#include <QDebug>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;

class MarkerSearchEngine : public QObject
{
    Q_OBJECT
public:
    explicit MarkerSearchEngine(QObject *parent = 0);
    int searchMarker(Mat &undistortMat);

private:
    Mat m_image;

    QList<QList<Point> > findMarkerRectangles();
    void drawMarkers(Mat &image);
    int decodeMarkers();

signals:

public slots:

};

#endif // MARKERSEARCHENGINE_H
