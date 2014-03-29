#ifndef POSEESTIMATIONENGINE_H
#define POSEESTIMATIONENGINE_H

#include <QObject>
#include <QDebug>
#include <QList>

#include <QMatrix>
#include <QMatrix4x4>

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
    QList<Marker> m_markerList;

    vector<Point2f> m_markerPoints;
    QMatrix4x4 m_robotSystemTransformationMatrix;

    // Points for drawing
    vector<Point2f> m_robotSystemCoordinatePoints;
    Point2f m_robotSystemCenter;


    QMatrix4x4 estimateRobotPosition();
    void drawRobotCoordinateSystem(Mat &img, Point2f center, vector<Point2f> coordinateSystemPoints);

    Point2f calculateCoordinateSystemCenter(Point2f p1, Point2f p2 ,Point2f p3, Point2f p4);

signals:

private slots:

public slots:

};

#endif // POSEESTIMATIONENGINE_H
