#ifndef MARKERSEARCHENGINE_H
#define MARKERSEARCHENGINE_H

#include <QObject>
#include <QList>
#include <QDebug>
#include <QList>

#include <QMatrix>
#include <QGenericMatrix>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

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

private:
    Mat m_grayMat;
    Mat m_resultMat;

    QList<Marker> m_markerList;
    vector<vector<Point2f> > m_rectangleList;

    void findRectangles();
    int decodeMarker(vector<Point2f> rectangle);
    Mat rotate(Mat matrix);

    int decodeBin();
    QGenericMatrix<1,7,int> correctCode(QGenericMatrix<1,7,int> code);
    int calculateId(QGenericMatrix<1,7,int> codeX, QGenericMatrix<1,7,int> codeY, QGenericMatrix<1,7,int> codeZ);
    
signals:

public slots:

};

#endif // MARKERSEARCHENGINE_H
