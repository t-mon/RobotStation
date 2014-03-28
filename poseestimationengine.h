#ifndef POSEESTIMATIONENGINE_H
#define POSEESTIMATIONENGINE_H

#include <QObject>
#include <QDebug>
#include <QList>

#include <QMatrix>
#include <QMatrix4x4>
#include <QGenericMatrix>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/calib3d/calib3d.hpp>

#include "markersearchengine.h"

using namespace cv;
using namespace std;

class PoseEstimationEngine : public QObject
{
    Q_OBJECT
public:
    explicit PoseEstimationEngine(QObject *parent = 0);
    void updateImage(Mat &image);


private:
    MarkerSearchEngine *m_markerSearchEngine;
    float m_markerSize;

    int m_imageWidth;
    int m_imageHeight;

    vector<Point2f> estimateMarkerPosition(Marker marker);
    void drawCoordinateSystem(Mat &img, Point2f markerCenter, vector<Point2f> coordinateSystemPoints);

signals:

private slots:

public slots:

};

#endif // POSEESTIMATIONENGINE_H
