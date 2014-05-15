#include "poseestimationengine.h"
#include "core.h"

PoseEstimationEngine::PoseEstimationEngine(QObject *parent) :
    QObject(parent)
{
    m_markerSearchEngine = new MarkerSearchEngine(this);
    m_debug = false;
    loadOffsetParameter();
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
//        qDebug() << "========================================";
//        qDebug() << "found " << m_markerList.count() << "marker";
//        qDebug() << "----------------------------------------";
//        foreach (const Marker &marker, m_markerList){
//            qDebug() << "ID             = " << marker.id();
//            qDebug() << "Center Point   = (" << marker.center().x << "," << marker.center().y << ")";
//            qDebug() << "----------------------------------------";
//        }

        // if we have found 4 Marker in the image...
        // make the same with a rectangle of the 4 marker center and estiate position

        if(m_markerList.count() == 4){

            // check if we have found the right markers for our detection
            foreach (Marker marker, m_markerList) {
                if(marker.id() != 111 && marker.id() != 222 && marker.id() != 333 && marker.id() != 444){
                    return;
                }
            }

            QMatrix4x4 rotateSystemTransformationMatrix(1,0,0,0,
                                                        0,-1,0,0,
                                                        0,0,-1,0,
                                                        0,0,0,1);


            m_robotSystemTransformationMatrix = estimateRobotPosition();
            qDebug() << m_robotSystemTransformationMatrix;
            emit coordinateSystemFound(m_robotSystemTransformationMatrix);

            m_robotSystemCenter = calculateCoordinateSystemCenter(m_markerPoints.at(0), m_markerPoints.at(1), m_markerPoints.at(2), m_markerPoints.at(3));
            drawRobotCoordinateSystem(image,m_robotSystemCenter,m_robotSystemCoordinatePoints);


            if(m_debug){
                //===================================================================
                imwrite("/home/timon/algorithmus_8.png",image);
            }
        }
    }
}

QMatrix4x4 PoseEstimationEngine::estimateRobotPosition()
{
    QMatrix4x4 transformationMatrix;
    transformationMatrix.setToIdentity();

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

    // Magic...? TODO: understand
    solvePnP(objectPoints,imagePoints,intrinsic,extrinsic,rotationOutput,translationOutput);

    // convert the rotation and tranlation vectors to double
    rotationOutput.convertTo(rotationVector,CV_32F);
    translationOutput.convertTo(translationVector,CV_32F);

    // create a rotation matrix from the euclidean rotation vector
    Rodrigues(rotationVector,rotationMatrix);

    // copy to rotation matrix into transformationmatrix
    for(int col = 0; col < 3; col++){
        for (int row = 0; row < 3; row++){
            transformationMatrix(row,col) = rotationMatrix(row,col);
        }
    }
    // copy to translation vector into transformationmatrix
    for(int row = 0; row < 3; row++){
        transformationMatrix(row,3) = translationOutput.at<double>(row);
    }

    // invert because solvePnP calculates camera positon, nor object position
    transformationMatrix = transformationMatrix.inverted();
    //qDebug() << "---------------------------------\n transformationMatrix = " << transformationMatrix;

    // calculate robot coodinate system points for dawing
    // set coordinate system points
    vector<Point3f> coordinateSytemPoints;
    coordinateSytemPoints.push_back(Point3f(80, 0, 0)); // x-axis
    coordinateSytemPoints.push_back(Point3f(0, -80, 0)); // y-axis
    coordinateSytemPoints.push_back(Point3f(0, 0, -80)); // z-axis

    vector<Point2f> imageCoordinateSystemPoints;
    projectPoints(coordinateSytemPoints,rotationVector,translationVector,intrinsic,extrinsic,imageCoordinateSystemPoints);
    m_robotSystemCoordinatePoints = imageCoordinateSystemPoints;

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


