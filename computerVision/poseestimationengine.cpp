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

#include "poseestimationengine.h"
#include "core.h"
#include <math.h>

PoseEstimationEngine::PoseEstimationEngine(QObject *parent) :
    QObject(parent)
{
    m_markerSearchEngine = new MarkerSearchEngine(this);
    m_debug = false;
    loadOffsetParameter();

    connect(this,SIGNAL(transformationVectorReady(QVector3D)),Core::instance()->window(),SLOT(updateTranslationVector(QVector3D)));
    connect(this,SIGNAL(eulerAnglesReady(QVector3D)),Core::instance()->window(),SLOT(updateRotationVector(QVector3D)));
}

void PoseEstimationEngine::updateImage(Mat &image)
{
    m_markerList.clear();
    m_markerList = m_markerSearchEngine->searchMarker(image);

    if(m_debug){
        //===================================================================
        Mat markerImage = image.clone();
        m_markerSearchEngine->drawMarkers(markerImage,m_markerList);
        imwrite("/home/timon/algorithmus_7.png",markerImage);
    }

    if(m_markerList.count() != 0){

        if(m_markerList.count() == 4){

            // check if we have found the right markers for our detection
            foreach (Marker marker, m_markerList) {
                if(marker.id() != 111 && marker.id() != 222 && marker.id() != 333 && marker.id() != 444){
                    return;
                }
            }

            // JUHU we have found the robot in pose -> robotcoordinatesystem
            m_robotSystemTransformationMatrix = estimateRobotPosition();
            emit coordinateSystemFound(m_robotSystemTransformationMatrix);

            // drawing
            m_robotDrawingSystemCenter = calculateCoordinateSystemCenter(m_markerPoints.at(0), m_markerPoints.at(1), m_markerPoints.at(2), m_markerPoints.at(3));
            drawOpticalCenter(image);
            drawRobotCoordinateSystem(image,m_robotDrawingSystemCenter,m_robotSystemCoordinateDrawingPoints);


            if(m_debug){
                //===================================================================
                imwrite("/home/timon/algorithmus_8.png",image);
            }
        }
    }
}

QMatrix4x4 PoseEstimationEngine::calculateTransformationFromPoint(QVector3D transformation, QVector3D rotation)
{
    // deg to rad
    float g = rotation.x() * M_PI / 180;
    float b = rotation.y() * M_PI / 180;
    float a = rotation.z() * M_PI / 180;

    /*   t11 t21 t31 dx
     *   t12 t22 t32 dy
     *   t13 t23 t33 dz
     *    0   0   0   1
     */

    float t11 = roundValue(cos(a)*cos(b));
    float t12 = roundValue(sin(a)*cos(b));
    float t13 = roundValue(-sin(b));

    float t21 = roundValue(cos(a)*sin(b)*sin(g)) - (sin(a)*cos(g));
    float t22 = roundValue((sin(a)*sin(b)*sin(g)) + (cos(a)*cos(g)));
    float t23 = roundValue((cos(b)*sin(g)));

    float t31 = roundValue((cos(a)*sin(b)*cos(g)) + (sin(a)*sin(g)));
    float t32 = roundValue((sin(a)*sin(b)*cos(g)) - (cos(a)*sin(g)));
    float t33 = roundValue((cos(b)*cos(g)));

    QMatrix4x4 trans(t11, t21, t31, transformation.x(),
                     t12, t22, t32, transformation.y(),
                     t13, t23, t33, transformation.z(),
                     0,   0,   0,   1);
    return trans;
}

QVector3D PoseEstimationEngine::calculateTranslationFromTransformation(QMatrix4x4 transformation)
{
    return QVector3D(transformation(0,3),transformation(1,3),transformation(2,3));
}

QVector3D PoseEstimationEngine::calculateRotationFromTransformation(QMatrix4x4 transformation)
{
    float u = atan2(transformation(1,0), transformation(0,0));     // x-axis
    float v = atan2(-transformation(2,0), sqrt( pow(transformation(2,1),2) +  pow(transformation(2,2),2) ));    // y-axis
    float w = atan2(transformation(2,1), transformation(2,2));      // z-axis

    QVector3D rotation;
    rotation.setX(u * 180 / M_PI);
    rotation.setY(v * 180 / M_PI);
    rotation.setZ(w * 180 / M_PI);

    return rotation;
}

float PoseEstimationEngine::roundValue(float val)
{
    return ((int)(val * 10000 + .5) / 10000.0);
}

QMatrix4x4 PoseEstimationEngine::calculateTransformationFromPoint()
{
    // deg to rad
    float g = (float)m_wx * M_PI / 180;
    float b = (float)m_wy * M_PI / 180;
    float a = (float)m_wz * M_PI / 180;

    /*   t11 t21 t31 dx
     *   t12 t22 t32 dy
     *   t13 t23 t33 dz
     *    0   0   0   1
     */

    float t11 = roundValue(cos(a)*cos(b));
    float t12 = roundValue(sin(a)*cos(b));
    float t13 = roundValue(-sin(b));

    float t21 = roundValue(cos(a)*sin(b)*sin(g)) - (sin(a)*cos(g));
    float t22 = roundValue((sin(a)*sin(b)*sin(g)) + (cos(a)*cos(g)));
    float t23 = roundValue((cos(b)*sin(g)));

    float t31 = roundValue((cos(a)*sin(b)*cos(g)) + (sin(a)*sin(g)));
    float t32 = roundValue((sin(a)*sin(b)*cos(g)) - (cos(a)*sin(g)));
    float t33 = roundValue((cos(b)*cos(g)));

    QMatrix4x4 trans(t11, t21, t31, m_dx,
                     t12, t22, t32, m_dy,
                     t13, t23, t33, m_dz,
                     0,   0,   0,   1);
    return trans;
}

QMatrix4x4 PoseEstimationEngine::estimateRobotPosition()
{

    // Model points of marker center points
    /*
     *   |----- 220 [mm] ----|
     *
     *  111-----------------444  --
     *   |                   |    |
     *   |                   |    |
     *   |                   |   130 [mm]
     *   |                   |    |
     *   |                   |    |
     *  222-----------------333  --
     *
     */

    float width  = 220;   // [mm]
    float height = 130;   // [mm]

    vector<Point3f> objectPoints;
    objectPoints.push_back(Point3f( -width/2, -height/2, 0));    // 111
    objectPoints.push_back(Point3f( -width/2,  height/2, 0));    // 222
    objectPoints.push_back(Point3f(  width/2,  height/2, 0));    // 333
    objectPoints.push_back(Point3f(  width/2, -height/2, 0));    // 444

    m_markerPoints.clear();

    // copy marker center points to the image points vector
    foreach(Marker marker, m_markerList){
        if(marker.id() == 111){
            m_markerPoints.push_back(marker.center());
        }
    }
    foreach(Marker marker, m_markerList){
        if(marker.id() == 222){
            m_markerPoints.push_back(marker.center());
        }
    }
    foreach(Marker marker, m_markerList){
        if(marker.id() == 333){
            m_markerPoints.push_back(marker.center());
        }
    }
    foreach(Marker marker, m_markerList){
        if(marker.id() == 444){
            m_markerPoints.push_back(marker.center());
        }
    }

    vector<Point2f> imagePoints = m_markerPoints;

    // get calibration parameters
    Mat intrinsic = Core::instance()->imageProcessor()->getIntrinsic();
    Mat extrinsic = Core::instance()->imageProcessor()->getExtrinsic();

    Mat_<float> rotationMatrix(3,3);

    Mat rotationVector;
    Mat translationVector;

    Mat rotationOutput;
    Mat translationOutput;

    solvePnP(objectPoints,imagePoints,intrinsic,extrinsic,rotationOutput,translationOutput);

    // convert the rotation and tranlation vectors to double
    rotationOutput.convertTo(rotationVector,CV_32F);
    translationOutput.convertTo(translationVector,CV_32F);

    // create a rotation matrix from the euclidean rotation vector
    Rodrigues(rotationVector,rotationMatrix);

    float r11 = rotationMatrix(0,0);
    float r21 = rotationMatrix(1,0);
    float r31 = rotationMatrix(2,0);

    float r12 = rotationMatrix(0,1);
    float r22 = rotationMatrix(1,1);
    float r32 = rotationMatrix(2,1);

    float r13 = rotationMatrix(0,2);
    float r23 = rotationMatrix(1,2);
    float r33 = rotationMatrix(2,2);

    // copy to translation vector into transformationmatrix

    float t1 = translationOutput.at<double>(0);
    float t2 = translationOutput.at<double>(1);
    float t3 = translationOutput.at<double>(2);


    QMatrix4x4 transformationMatrix(r11, r12, r13, t1,
                                    r21, r22, r23, t2,
                                    r31, r32, r33, t3,
                                     0,   0,   0,   1);

    // invert because solvePnP calculates camera positon, nor object position
    transformationMatrix = transformationMatrix.inverted();

    // calculate offsets
    qDebug() << "===================================================";
    QMatrix4x4 offsetTransformationMatrix = calculateTransformationFromPoint();
    qDebug() << offsetTransformationMatrix << m_robotSystemTransformationMatrix;
    m_robotSystemTransformationMatrix = transformationMatrix * calculateTransformationFromPoint();
    qDebug() << "===================================================";

    // calculate transformation vector
    QVector3D coordinateSytemRealCenter;
    coordinateSytemRealCenter.setX(roundValue(m_robotSystemTransformationMatrix(0,3)));
    coordinateSytemRealCenter.setY(roundValue(m_robotSystemTransformationMatrix(1,3)));
    coordinateSytemRealCenter.setZ(roundValue(m_robotSystemTransformationMatrix(2,3)));
    qDebug() << coordinateSytemRealCenter;
    emit transformationVectorReady(coordinateSytemRealCenter);

    // calculate euler angles vector
    QVector3D coordinateSytemRealEuler = calculateEuler();
    qDebug() << coordinateSytemRealEuler;
    emit eulerAnglesReady(coordinateSytemRealEuler);

    // calculate robot coodinate system points for dawing
    // set coordinate system points
    vector<Point3f> coordinateSytemObjectPoints;
    coordinateSytemObjectPoints.push_back(Point3f(80, 0, 0)); // x-axis
    coordinateSytemObjectPoints.push_back(Point3f(0, -80, 0)); // y-axis
    coordinateSytemObjectPoints.push_back(Point3f(0, 0, -80)); // z-axis

    vector<Point2f> imageCoordinateSystemPoints;
    projectPoints(coordinateSytemObjectPoints,rotationVector,translationVector,intrinsic,extrinsic,imageCoordinateSystemPoints);
    m_robotSystemCoordinateDrawingPoints = imageCoordinateSystemPoints;

    return transformationMatrix;
}

void PoseEstimationEngine::drawRobotCoordinateSystem(Mat &img, Point2f center, vector<Point2f> coordinateSystemPoints)
{

    int fontFace = FONT_HERSHEY_PLAIN;
    double fontScale = 1.5;
    int thickness = 2;

    line(img,center, coordinateSystemPoints.at(0),Scalar(0,0,255),2,8);   // x-axis
    putText(img, "x", coordinateSystemPoints.at(0), fontFace, fontScale, Scalar(0,0,255), thickness, 8);

    line(img,center, coordinateSystemPoints.at(1),Scalar(0,255,0),2,8);   // y-axis
    putText(img, "y", coordinateSystemPoints.at(1), fontFace, fontScale, Scalar(0,255,0), thickness, 8);

    line(img,center, coordinateSystemPoints.at(2),Scalar(255,0,0),2,8);   // z-axis
    putText(img, "z", coordinateSystemPoints.at(2), fontFace, fontScale, Scalar(255,0,0), thickness, 8);

}

void PoseEstimationEngine::drawOpticalCenter(Mat &img)
{
    circle(img,Core::instance()->imageProcessor()->getOpticalCenter(),3,Scalar(0,255,0),2,8);
}

Point2f PoseEstimationEngine::calculateCoordinateSystemCenter(Point2f p1, Point2f p2, Point2f p3, Point2f p4)
{
    float a1 = p3.x - p1.x;
    float b1 = p3.y - p1.y;
    float a2 = p4.x - p2.x;
    float b2 = p4.y - p2.y;

    float s = (a1*p2.y - a1*p1.y - b1*p2.x + b1*p1.x) / (b1*a2 - b2*a1);
    float t = (p2.x + s*a2 -p1.x)/(a1);

    Point2f centerPoint1 = p1 + t * (p3 - p1);
    Point2f centerPoint2 = p2 + s * (p4 - p2);

    // centerPoint1 and centerPoint2 have to be equal...otherwise there is no center
    // if we have a small error from floating overload...compensate it with average x- and y- value
    Point2f centerPoint;
    centerPoint.x = (centerPoint1.x + centerPoint2.x) / 2;
    centerPoint.y = (centerPoint1.y + centerPoint2.y) / 2;

    return centerPoint;
}

QVector3D PoseEstimationEngine::calculateEuler()
{

    float roll = atan2(m_robotSystemTransformationMatrix(1,0), m_robotSystemTransformationMatrix(0,0));     // x-axis
    float pitch = atan2(-m_robotSystemTransformationMatrix(2,0), sqrt( pow(m_robotSystemTransformationMatrix(2,1),2) +  pow(m_robotSystemTransformationMatrix(2,2),2) ));    // y-axis
    float yaw = atan2(m_robotSystemTransformationMatrix(2,1), m_robotSystemTransformationMatrix(2,2));      // z-axis

    QVector3D eulerAngles;
    eulerAngles.setX(roundValue(pitch * 180 / M_PI));
    eulerAngles.setY(roundValue(yaw * 180 / M_PI));
    eulerAngles.setZ(roundValue(roll * 180 / M_PI));

    return eulerAngles;
}

void PoseEstimationEngine::loadOffsetParameter()
{
    QSettings settings("RobotStation");
    settings.beginGroup("TCP Offset");
    m_dx = settings.value("dx",0).toInt();
    m_dy = settings.value("dy",0).toInt();
    m_dz = settings.value("dz",0).toInt();
    m_wx = settings.value("wx",0).toInt();
    m_wy = settings.value("wy",0).toInt();
    m_wz = settings.value("wz",0).toInt();
    settings.endGroup();

    qDebug() << "pose engine offset parameter loaded";
    qDebug() << "dx:" << m_dx;
    qDebug() << "dy:" << m_dy;
    qDebug() << "dz:" << m_dz;
    qDebug() << "wx:" << m_wx;
    qDebug() << "wy:" << m_wy;
    qDebug() << "wz:" << m_wz;
}


