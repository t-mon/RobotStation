#ifndef MARKER_H
#define MARKER_H

#include <QObject>
#include <QMatrix4x4>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;
using namespace std;

class Marker
{
public:
    Marker(Point2f p1 = Point(), Point2f p2 = Point(), Point2f p4 = Point(), Point2f p3 = Point());

    void setP1(const Point2f &p1);
    Point2f p1() const;

    void setP2(const Point2f &p2);
    Point2f p2() const;

    void setP3(const Point2f &p3);
    Point2f p3() const;

    void setP4(const Point2f &p4);
    Point2f p4() const;

    void setCenter(const Point2f &center);
    Point2f center() const;

    void setCoordinateSystemPoints(const vector<Point2f> &points);
    vector<Point2f> coordinateSystemPoints() const;

    void setId(const int &id);
    int id() const;

    void setRotation(const int rotation);
    int roation() const;

    void setTransformationMatrix(const QMatrix4x4 &matrix);
    QMatrix4x4 transformationMatrix();

private:
    Point2f m_p1;
    Point2f m_p2;
    Point2f m_p3;
    Point2f m_p4;

    Point2f m_center;
    vector<Point2f> m_coordinateSystemPoints;
    int m_id;
    int m_rotation;
    QMatrix4x4 m_transformationMatrix;
};

#endif // MARKER_H
