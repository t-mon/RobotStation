/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  Copyright 2014 Simon Stuerz                                            *
 *  This file is part of RobotStation.                                     *
 *                                                                         *
 *  RobotStation is free software: you can redistribute it and/or modify   *
 *  it under the terms of the GNU General Public License as published by   *
 *  the Free Software Foundation, version 2 of the License.                *
 *                                                                         *
 *  RobotStation is distributed in the hope that it will be useful         *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the           *
 *  GNU General Public License for more details.                           *
 *                                                                         *
 *  You should have received a copy of the GNU General Public License      *
 *  along with RobotStation. If not, see <http://www.gnu.org/licenses/>.   *
 *                                                                         *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

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
