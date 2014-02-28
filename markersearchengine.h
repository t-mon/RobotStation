#ifndef MARKERSEARCHENGINE_H
#define MARKERSEARCHENGINE_H

#include <QObject>
#include <QList>
#include <QDebug>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;
using namespace std;

class MarkerSearchEngine : public QObject
{
    Q_OBJECT
public:
    explicit MarkerSearchEngine(QObject *parent = 0);
    int searchMarker(Mat &undistortMat);

private:
    Mat m_grayMat;
    Mat m_resultMat;

    vector<vector<Point2f> > m_markerList;
    vector<vector<Point2f> > m_rectangleList;

    void findRectangles();
    int decodeMarker(vector<Point2f> rectangle);
    void estimateMarkerPositions();
    void drawMarkers(Mat &image);

    float perimeter(vector<Point2f> rectangle);
    int hammDistMarker(Mat bits);
    Mat rotate(Mat matrix);
signals:

public slots:

};

#endif // MARKERSEARCHENGINE_H
