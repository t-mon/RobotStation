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
    QMatrix4x4 calculateTransformationFromPoint(QVector3D transformation, QVector3D rotation);

    QVector3D calculateTranslationFromTransformation(QMatrix4x4 transformation);
    QVector3D calculateRotationFromTransformation(QMatrix4x4 transformation);

    float roundValue(float val);

private:
    // Offset Parameter
    int m_dx;
    int m_dy;
    int m_dz;
    int m_wx;
    int m_wy;
    int m_wz;

    bool m_debug;

    MarkerSearchEngine *m_markerSearchEngine;
    QList<Marker> m_markerList;

    vector<Point2f> m_markerPoints;
    QMatrix4x4 m_robotSystemTransformationMatrix;

    // Points for drawing
    vector<Point2f> m_robotSystemCoordinateDrawingPoints;
    Point2f m_robotDrawingSystemCenter;

    QMatrix4x4 calculateTransformationFromPoint();

    QMatrix4x4 estimateRobotPosition();
    void drawRobotCoordinateSystem(Mat &img, Point2f center, vector<Point2f> coordinateSystemPoints);
    void drawOpticalCenter(Mat &img);

    Point2f calculateCoordinateSystemCenter(Point2f p1, Point2f p2 ,Point2f p3, Point2f p4);

    QVector3D calculateEuler();


signals:
    void coordinateSystemFound(const QMatrix4x4 &transformationMatrix);
    void transformationVectorReady(const QVector3D &transformation);
    void eulerAnglesReady(const QVector3D &euler);

private slots:

public slots:
    void loadOffsetParameter();

};

#endif // POSEESTIMATIONENGINE_H
