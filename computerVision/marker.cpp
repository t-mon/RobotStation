#include "marker.h"

Marker::Marker(Point2f p1, Point2f p2, Point2f p4, Point2f p3) :
    m_p1(p1),m_p2(p2),m_p3(p3),m_p4(p4)
{
    m_center = Point2f();
    m_id = -1;
    m_rotation = 0;
    m_transformationMatrix.setToIdentity();
}

void Marker::setP1(const Point2f &p1)
{
    m_p1 = p1;
}

Point2f Marker::p1() const
{
    return m_p1;
}

void Marker::setP2(const Point2f &p2)
{
    m_p2 = p2;
}

Point2f Marker::p2() const
{
    return m_p2;
}

void Marker::setP3(const Point2f &p3)
{
    m_p3 = p3;
}

Point2f Marker::p3() const
{
    return m_p3;
}

void Marker::setP4(const Point2f &p4)
{
    m_p4 = p4;
}

Point2f Marker::p4() const
{
    return m_p4;
}

void Marker::setCenter(const Point2f &center)
{
    m_center = center;
}

Point2f Marker::center() const
{
    return m_center;
}

void Marker::setCoordinateSystemPoints(const vector<Point2f> &points)
{
    m_coordinateSystemPoints = points;
}

vector<Point2f> Marker::coordinateSystemPoints() const
{
    return m_coordinateSystemPoints;
}

void Marker::setId(const int &id)
{
    m_id = id;
}

int Marker::id() const
{
    return m_id;
}

void Marker::setRotation(const int rotation)
{
    m_rotation = rotation;
}

int Marker::roation() const
{
    return m_rotation;
}

void Marker::setTransformationMatrix(const QMatrix4x4 &matrix)
{
    m_transformationMatrix = matrix;
}

QMatrix4x4 Marker::transformationMatrix()
{
    return m_transformationMatrix;
}
