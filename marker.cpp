#include "marker.h"

Marker::Marker(Point2f p1, Point2f p2, Point2f p4, Point2f p3) :
    m_p1(p1),m_p2(p2),m_p3(p3),m_p4(p4)
{
}

Point2f Marker::p1() const
{
    return m_p1;
}

Point2f Marker::p2() const
{
    return m_p2;
}

Point2f Marker::p3() const
{
    return m_p3;
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

void Marker::setId(const int &id)
{
    m_id = id;
}

int Marker::id() const
{
    return m_id;
}
