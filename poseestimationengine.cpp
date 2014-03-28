#include "poseestimationengine.h"
#include "core.h"

PoseEstimationEngine::PoseEstimationEngine(QObject *parent) :
    QObject(parent)
{
    m_markerSearchEngine = new MarkerSearchEngine(this);
    m_markerSize = 50; //[mm]
}

void PoseEstimationEngine::updateImage(Mat &image)
{
    m_imageWidth = image.cols;
    m_imageHeight = image.rows;

    QList<Marker> markerList = m_markerSearchEngine->searchMarker(image);
    m_markerSearchEngine->drawMarkers(image,markerList);

    if(markerList.count() != 0){
        qDebug() << "========================================";
        qDebug() << "found " << markerList.count() << "marker";
        qDebug() << "----------------------------------------";
        foreach (const Marker &marker, markerList){
            vector<Point2f> coordinateSystem = estimateMarkerPosition(marker);
            drawCoordinateSystem(image,marker.center(),coordinateSystem);
            qDebug() << "ID             = " << marker.id();
            qDebug() << "Center Point   = (" << marker.center().x << "," << marker.center().y << ")";
            qDebug() << "----------------------------------------";
        }

        // if we have found 4 Marker in the image...
        // make the same with a rectangle of the 4 marker center and estiate position

        if(markerList.count() == 4){
            // first check if we have found our 4 marker


        }
    }
}

vector<Point2f> PoseEstimationEngine::estimateMarkerPosition(Marker marker)
{
    // set object points (virtual marker in the center of the image wit a side leght of m_markerSize)
    vector<Point3f> objectPoints;

    objectPoints.push_back(Point3f( -m_markerSize/2, -m_markerSize/2, 0));
    objectPoints.push_back(Point3f( -m_markerSize/2,  m_markerSize/2, 0));
    objectPoints.push_back(Point3f(  m_markerSize/2,  m_markerSize/2, 0));
    objectPoints.push_back(Point3f(  m_markerSize/2, -m_markerSize/2, 0));

    // set image points (detected marker corners)
    vector<Point2f> imagePoints;
    imagePoints.push_back(marker.p1());
    imagePoints.push_back(marker.p2());
    imagePoints.push_back(marker.p3());
    imagePoints.push_back(marker.p4());

    // set coordinate system points
    vector<Point3f> coordinateSytemPoints;
    coordinateSytemPoints.push_back(Point3f(m_markerSize/2, 0, 0)); // x-axis
    coordinateSytemPoints.push_back(Point3f(0, m_markerSize/2, 0)); // y-axis
    coordinateSytemPoints.push_back(Point3f(0, 0, m_markerSize/2)); // z-axis

    // get calibration parameters
    Mat intrinsic = Core::instance()->imageProcessor()->getIntrinsic();
    Mat extrinsic = Core::instance()->imageProcessor()->getExtrinsic();

    Mat_<float> rotationMatrix(3,3);

    Mat rotationVector;
    Mat translationVector;

    Mat rotationOutput;
    Mat translationOutput;

    QMatrix4x4 transformationMatrix;
    transformationMatrix.setToIdentity();

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

    marker.setTransformationMatrix(transformationMatrix);

    // calculate coodinate system points for dawing
    vector<Point2f> imageCoordinateSystemPoints;
    projectPoints(coordinateSytemPoints,rotationVector,translationVector,intrinsic,extrinsic,imageCoordinateSystemPoints);

    return imageCoordinateSystemPoints;
}

void PoseEstimationEngine::drawCoordinateSystem(Mat &img, Point2f markerCenter, vector<Point2f> coordinateSystemPoints)
{
    line(img,markerCenter, coordinateSystemPoints.at(0),Scalar(0,0,255),2,8);   // x-axis
    line(img,markerCenter, coordinateSystemPoints.at(1),Scalar(0,255,0),2,8);   // y-axis
    line(img,markerCenter, coordinateSystemPoints.at(2),Scalar(255,0,0),2,8);   // z-axis
}


