#ifndef MARKER_H
#define MARKER_H

#include <QObject>
#include <QMatrix4x4>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;

class Marker
{
public:
    Marker(Point2f p1, Point2f p2, Point2f p4, Point2f p3);

    Point2f p1() const;
    Point2f p2() const;
    Point2f p3() const;
    Point2f p4() const;

    void setCenter(const Point2f &center);
    Point2f center() const;

    void setId(const int &id);
    int id() const;

    void setTransformationMatrix(const QMatrix4x4 &matrix);
    QMatrix4x4 transformationMatrix();

private:
    Point2f m_p1;
    Point2f m_p2;
    Point2f m_p3;
    Point2f m_p4;

    Point2f m_center;
    int m_id;
    QMatrix4x4 m_transformationMatrix;
};

#endif // MARKER_H
