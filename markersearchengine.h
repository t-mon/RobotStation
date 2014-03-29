#ifndef MARKERSEARCHENGINE_H
#define MARKERSEARCHENGINE_H

#include <QObject>
#include <QList>
#include <QDebug>
#include <QList>

#include <QMatrix>
#include <QMatrix4x4>
#include <QGenericMatrix>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/calib3d/calib3d.hpp>

#include "marker.h"

using namespace cv;
using namespace std;

class MarkerSearchEngine : public QObject
{
    Q_OBJECT
public:
    explicit MarkerSearchEngine(QObject *parent = 0);
    QList<Marker> searchMarker(Mat &undistortMat);
    void drawMarkers(Mat &image, QList<Marker> markerList);
    void  drawMarkerCoordinateSystem(Mat &img, Point2f markerCenter, vector<Point2f> coordinateSystemPoints);


private:
    Mat m_undistortMat;
    Mat m_grayMat;
    Mat m_resultMat;

    QList<Marker> m_markerList;
    vector<vector<Point2f> > m_rectangleList;
    vector<Point2f> m_coordinateSystemPoints;

    bool m_debug;
    int m_rotationCounter;

    void findRectangles();
    void drawRectangel(Mat &image, vector<Point2f> rectangle);

    int decodeMarker(vector<Point2f> rectangle);
    Mat rotate(Mat matrix);
    Marker rotateMarkerPoints(Marker marker);

    QGenericMatrix<1,7,int> correctCode(QGenericMatrix<1,7,int> code);
    int calculateId(QGenericMatrix<1,7,int> codeX, QGenericMatrix<1,7,int> codeY, QGenericMatrix<1,7,int> codeZ);
    QMatrix4x4 estimateMarkerPose(Marker marker);

signals:

public slots:

};

#endif // MARKERSEARCHENGINE_H
