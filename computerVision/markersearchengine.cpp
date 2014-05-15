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

#include "markersearchengine.h"
#include "core.h"

MarkerSearchEngine::MarkerSearchEngine(QObject *parent) :
    QObject(parent)
{
    m_debug = false;
}

QList<Marker> MarkerSearchEngine::searchMarker(Mat &undistortMat)
{
    // clean up old data
    m_rectangleList.clear();
    m_markerList.clear();

    m_undistortMat = undistortMat;
    cvtColor(m_undistortMat,m_grayMat,CV_BGR2GRAY);

    // first find all rectangles
    findRectangles();

    // then take try to decode the rectangle to check if its a marker
    foreach (const vector<Point2f> rectangle, m_rectangleList){
        decodeMarker(rectangle);
    }
    return m_markerList;
}

void MarkerSearchEngine::drawMarkers(Mat &image, QList<Marker> markerList)
{
    int fontFace = FONT_HERSHEY_SCRIPT_SIMPLEX;
    double fontScale = 0.6;
    int thickness = 2;

    foreach (const Marker &marker, markerList){
        // Draw border lines
        line(image,marker.p1(),marker.p2(),Scalar(0, 255, 0),1,8,0);
        line(image,marker.p2(),marker.p3(),Scalar(0, 255, 0),1,8,0);
        line(image,marker.p3(),marker.p4(),Scalar(0, 255, 0),1,8,0);
        line(image,marker.p4(),marker.p1(),Scalar(0, 255, 0),1,8,0);

        // draw border points
        circle(image,marker.p1(),5,Scalar(0,0,255),1,8);
        circle(image,marker.p2(),5,Scalar(0,0,255),1,8);
        circle(image,marker.p3(),5,Scalar(0,0,255),1,8);
        circle(image,marker.p4(),5,Scalar(0,0,255),1,8);

        // draw diagonales
        line(image,marker.p1(),marker.p3(),Scalar(0, 255, 0),1,8,0);
        line(image,marker.p2(),marker.p4(),Scalar(0, 255, 0),1,8,0);

        // draw center point
        circle(image,marker.center(),3,Scalar(0,0,255),1,8);

        // draw center of image
        //        circle(image,Point2f(image.cols/2, image.rows/2),1,Scalar(0,0,255),3,8);

        //write id as text to center of marker
        string text = QString::number(marker.id()).toStdString();
        Point2f drawPoint = Point2f(marker.center().x+20,marker.center().y);
        putText(image, text, drawPoint, fontFace, fontScale, Scalar(130, 255, 80), thickness, 8);

        drawMarkerCoordinateSystem(image,marker.center(),marker.coordinateSystemPoints());
    }
}

void MarkerSearchEngine::drawMarkerCoordinateSystem(Mat &img, Point2f markerCenter, vector<Point2f> coordinateSystemPoints)
{
    line(img, markerCenter, coordinateSystemPoints.at(0),Scalar(0,0,255),2,8);   // x-axis
    line(img, markerCenter, coordinateSystemPoints.at(1),Scalar(0,255,0),2,8);   // y-axis
    line(img, markerCenter, coordinateSystemPoints.at(2),Scalar(255,0,0),2,8);   // z-axis
}

void MarkerSearchEngine::findRectangles()
{
    // Adaptive threshold
    Mat adaptiveMat;
    adaptiveThreshold(m_grayMat, adaptiveMat, 255,ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY_INV, 9, 9);

    // find contours
    Mat contourImage = adaptiveMat.clone();
    vector<vector<Point> > allContours;
    vector<Vec4i> hierarchy;
    findContours(contourImage, allContours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);

    // filter small contours away
    int minContourSizeAllowed = 50;
    vector<vector<Point> > contours;
    contours.clear();
    for(uint i = 0; i < allContours.size(); i++){
        int contourSize = allContours[i].size();
        if( contourSize > minContourSizeAllowed){
            contours.push_back(allContours[i]);
        }
    }

    // find convex rectangles
    vector<Point> approxCurve;
    vector<vector<Point2f> > possibleRectangles;
    possibleRectangles.clear();
    for(uint i = 0; i < contours.size(); i++){
        // search polygons
        double epsilon = contours[i].size() * 0.05;
        approxPolyDP(contours[i], approxCurve, epsilon, true);

        // if its a rectangle (polygon with 4 points)
        if (approxCurve.size() == 4){
            // and it's convenx
            if(isContourConvex(approxCurve)){
                // make sure that the distace between consecutive points is large enough
                int minDistAllowed = 20;
                float minDist = std::numeric_limits<float>::max();
                vector<Point2f> rectangle;
                for(int i = 0; i < 4; i++){
                    Point side = approxCurve[i] - approxCurve[(i+1)%4];
                    float squaredSideLength = side.dot(side);
                    minDist = min(minDist, squaredSideLength);
                    rectangle.push_back(Point(approxCurve[i].x, approxCurve[i].y));
                }
                if (minDist>minDistAllowed){
                    for(int i=0;i<4;i++){
                        possibleRectangles.push_back(rectangle);
                    }
                }
            }
        }
    }

    // now sort the rectangle points counter clockwise
    m_rectangleList.clear();
    foreach (vector<Point2f> rectangle, possibleRectangles){
        Point2f v1 = rectangle.at(1) - rectangle.at(0);
        Point2f v2 = rectangle.at(2) - rectangle.at(0);

        double d = (v1.x * v2.y) - (v1.y * v2.x);

        if(d < 0){
            swap(rectangle.at(1), rectangle.at(3));
        }

        bool allreadyExists = false;
        foreach (vector<Point2f> rec, m_rectangleList){
            if(rec == rectangle){
                allreadyExists = true;
            }
        }
        if(!allreadyExists){
            m_rectangleList.push_back(rectangle);
        }
    }

    if(m_debug){
        //===================================================================
        imwrite("/home/timon/algorithmus_0.png",m_undistortMat);

        //===================================================================
        imwrite("/home/timon/algorithmus_1.png",m_grayMat);

        //===================================================================
        // draw adaptiv threshold image
        imwrite("/home/timon/algorithmus_2.png",adaptiveMat);

        //===================================================================
        // Draw all contours
        Mat allContourImage = Mat::zeros( m_grayMat.size(), CV_8UC3 );
        for(uint i = 0; i< allContours.size(); i++){
            drawContours( allContourImage, allContours, i, Scalar(255,255,255), 1, 8, hierarchy, 0, Point() );
        }
        imwrite("/home/timon/algorithmus_3.png",allContourImage);

        //===================================================================
        // Draw contours
        Mat contourImage = Mat::zeros( m_grayMat.size(), CV_8UC3 );
        for(uint i = 0; i< contours.size(); i++){
            drawContours( contourImage, contours, i, Scalar(255,255,255), 1, 8, hierarchy, 0, Point() );
        }
        imwrite("/home/timon/algorithmus_4.png",contourImage);

        //===================================================================
        // Draw all rectangles
        Mat rectangleImage = m_undistortMat.clone();
        for(uint i = 0; i< m_rectangleList.size(); i++){
            drawRectangel(rectangleImage,m_rectangleList.at(i));
        }
        imwrite("/home/timon/algorithmus_5.png",rectangleImage);

    }

}

void MarkerSearchEngine::drawRectangel(Mat &image, vector<Point2f> rectangle)
{
    for(uint i = 0; i< 4; i++){
        line(image,rectangle.at(0),rectangle.at(1),Scalar(0,255,0),2,8);
        line(image,rectangle.at(1),rectangle.at(2),Scalar(0,255,0),2,8);
        line(image,rectangle.at(2),rectangle.at(3),Scalar(0,255,0),2,8);
        line(image,rectangle.at(3),rectangle.at(0),Scalar(0,255,0),2,8);
    }
}

int MarkerSearchEngine::decodeMarker(vector<Point2f> rectangle)
{
    Point2f points[4];
    Point2f pointsRectangle[4];

    for(uint i = 0; i < rectangle.size(); i++){
        pointsRectangle[i] = rectangle[i];
    }

    // size = 105x105 -> each sub square has 15 pixel...
    Size size = Size(105,105);
    points[0] = Point2f(0,0);
    points[1] = Point2f(size.width,0);
    points[2] = Point2f(size.width,size.height);
    points[3] = Point2f(0,size.height);

    Mat markerMat;
    Mat transformationMatrix = getPerspectiveTransform(pointsRectangle, points);
    warpPerspective(m_grayMat, markerMat, transformationMatrix, size);

    threshold(markerMat, markerMat, 125, 255, THRESH_BINARY | THRESH_OTSU);


    // first check if boarder is totaly black
    int cellSize = (int)((float) size.width / 7);
    for(int y = 0; y < 7; y++){
        int inc=6;
        if(y==0 || y==6){
            inc=1;
        }
        for(int x = 0; x < 7; x += inc){
            int Xstart=(x)*(cellSize);
            int Ystart=(y)*(cellSize);
            Mat square=markerMat(Rect(Xstart,Ystart,cellSize,cellSize));
            int nZ=countNonZero(square);
            if(nZ > (cellSize*cellSize) /2){
                //can not be a marker because the border element is not black!
                return -1;
            }
        }
    }

    if(m_debug){
        imwrite("/home/timon/algorithmus_6.png",markerMat);
    }

    //Rotate in readable orientation
    markerMat = rotate(markerMat);

    if(m_rotationCounter == 4){
        return -1;
    }

    /*  |-----|-----|-----|-----|-----|-----|-----|
     *  |  0  |  0  |  0  |  0  |  0  |  0  |  0  |
     *  |     |     |     |     |     |     |     |
     *  |-----|-----|-----|-----|-----|-----|-----|
     *  |  0  |  1  | Px1 | Dx1 | Px2 |  0  |  0  |
     *  |     |     |     |     |     |     |     |
     *  |-----|-----|-----|-----|-----|-----|-----|
     *  |  0  | Px3 | Dx2 | Dx3 | Dx4 | Py1 |  0  |
     *  |     |     |     |     |     |     |     |
     *  |-----|-----|-----|-----|-----|-----|-----|
     *  |  0  | Dy1 | Py2 | Py3 | Dy2 | Dy3 |  0  |
     *  |     |     |     |     |     |     |     |
     *  |-----|-----|-----|-----|-----|-----|-----|
     *  |  0  | Dy4 | Pz1 | Dz1 | Pz2 | Pz3 |  0  |
     *  |     |     |     |     |     |     |     |
     *  |-----|-----|-----|-----|-----|-----|-----|
     *  |  0  |  0  | Dz2 | Dz3 | Dz4 |  0  |  0  |
     *  |     |     |     |     |     |     |     |
     *  |-----|-----|-----|-----|-----|-----|-----|
     *  |  0  |  0  |  0  |  0  |  0  |  0  |  0  |
     *  |     |     |     |     |     |     |     |
     *  |-----|-----|-----|-----|-----|-----|-----|
     *
     *
     *  Code word -> | Dx1 | Dx2 | Dx3 | Dx4 | Dy1 | Dy2 | Dy3 | Dy4 | Dz1 | Dz2 | Dz3 | Dz4 | = 2^12 = maximal 4096
     *
     *  So we have: - frame has to be black (0)
     *              - 4  cells for orientation
     *              - 9  cells for pairity and correction
     *              - 12 cells for data
     */

    // check if inner squares are black or white

    // get datax, datay and dataz
    QGenericMatrix<1,7,int> codeX;
    QGenericMatrix<1,7,int> codeY;
    QGenericMatrix<1,7,int> codeZ;

    Mat cell;
    int nZ = 0;

    //==================================================
    // data x

    // p1
    cell = markerMat(Rect((2)* cellSize,(1)* cellSize,cellSize,cellSize));
    nZ=countNonZero(cell);
    if(nZ > (cellSize*cellSize) /2){
        codeX(0,0) = 1;
    }else{
        codeX(0,0) = 0;
    }

    // p2
    cell = markerMat(Rect((4)* cellSize,(1)* cellSize,cellSize,cellSize));
    nZ=countNonZero(cell);
    if(nZ > (cellSize*cellSize) /2){
        codeX(1,0) = 1;
    }else{
        codeX(1,0) = 0;
    }

    // d1
    cell = markerMat(Rect((3)* cellSize,(1)* cellSize,cellSize,cellSize));
    nZ=countNonZero(cell);
    if(nZ > (cellSize*cellSize) /2){
        codeX(2,0) = 1;
    }else{
        codeX(2,0) = 0;
    }


    // p3
    cell = markerMat(Rect((1)* cellSize,(2)* cellSize,cellSize,cellSize));
    nZ=countNonZero(cell);
    if(nZ > (cellSize*cellSize) /2){
        codeX(3,0) = 1;
    }else{
        codeX(3,0) = 0;
    }

    // d2
    cell = markerMat(Rect((2)* cellSize,(2)* cellSize,cellSize,cellSize));
    nZ=countNonZero(cell);
    if(nZ > (cellSize*cellSize) /2){
        codeX(4,0) = 1;
    }else{
        codeX(4,0) = 0;
    }

    // d3
    cell = markerMat(Rect((3)* cellSize,(2)* cellSize,cellSize,cellSize));
    nZ=countNonZero(cell);
    if(nZ > (cellSize*cellSize) /2){
        codeX(5,0) = 1;
    }else{
        codeX(5,0) = 0;
    }

    // d4
    cell = markerMat(Rect((4)* cellSize,(2)* cellSize,cellSize,cellSize));
    nZ=countNonZero(cell);
    if(nZ > (cellSize*cellSize) /2){
        codeX(6,0) = 1;
    }else{
        codeX(6,0) = 0;
    }

    //==================================================
    // data y

    // p1
    cell = markerMat(Rect((5)* cellSize,(2)* cellSize,cellSize,cellSize));
    nZ=countNonZero(cell);
    if(nZ > (cellSize*cellSize) /2){
        codeY(0,0) = 1;
    }else{
        codeY(0,0) = 0;
    }

    // p2
    cell = markerMat(Rect((2)* cellSize,(3)* cellSize,cellSize,cellSize));
    nZ=countNonZero(cell);
    if(nZ > (cellSize*cellSize) /2){
        codeY(1,0) = 1;
    }else{
        codeY(1,0) = 0;
    }

    // d1
    cell = markerMat(Rect((1)* cellSize,(3)* cellSize,cellSize,cellSize));
    nZ=countNonZero(cell);
    if(nZ > (cellSize*cellSize) /2){
        codeY(2,0) = 1;
    }else{
        codeY(2,0) = 0;
    }


    // p3
    cell = markerMat(Rect((3)* cellSize,(3)* cellSize,cellSize,cellSize));
    nZ=countNonZero(cell);
    if(nZ > (cellSize*cellSize) /2){
        codeY(3,0) = 1;
    }else{
        codeY(3,0) = 0;
    }

    // d2
    cell = markerMat(Rect((4)* cellSize,(3)* cellSize,cellSize,cellSize));
    nZ=countNonZero(cell);
    if(nZ > (cellSize*cellSize) /2){
        codeY(4,0) = 1;
    }else{
        codeY(4,0) = 0;
    }

    // d3
    cell = markerMat(Rect((5)* cellSize,(3)* cellSize,cellSize,cellSize));
    nZ=countNonZero(cell);
    if(nZ > (cellSize*cellSize) /2){
        codeY(5,0) = 1;
    }else{
        codeY(5,0) = 0;
    }

    // d4
    cell = markerMat(Rect((1)* cellSize,(4)* cellSize,cellSize,cellSize));
    nZ=countNonZero(cell);
    if(nZ > (cellSize*cellSize) /2){
        codeY(6,0) = 1;
    }else{
        codeY(6,0) = 0;
    }

    //qDebug() << "data y =" << codeY;

    //==================================================
    // data z

    // p1
    cell = markerMat(Rect((2)* cellSize,(4)* cellSize,cellSize,cellSize));
    nZ=countNonZero(cell);
    if(nZ > (cellSize*cellSize) /2){
        codeZ(0,0) = 1;
    }else{
        codeZ(0,0) = 0;
    }

    // p2
    cell = markerMat(Rect((4)* cellSize,(4)* cellSize,cellSize,cellSize));
    nZ=countNonZero(cell);
    if(nZ > (cellSize*cellSize) /2){
        codeZ(1,0) = 1;
    }else{
        codeZ(1,0) = 0;
    }

    // d1
    cell = markerMat(Rect((3)* cellSize,(4)* cellSize,cellSize,cellSize));
    nZ=countNonZero(cell);
    if(nZ > (cellSize*cellSize) /2){
        codeZ(2,0) = 1;
    }else{
        codeZ(2,0) = 0;
    }

    // p3
    cell = markerMat(Rect((5)* cellSize,(4)* cellSize,cellSize,cellSize));
    nZ=countNonZero(cell);
    if(nZ > (cellSize*cellSize) /2){
        codeZ(3,0) = 1;
    }else{
        codeZ(3,0) = 0;
    }

    // d2
    cell = markerMat(Rect((2)* cellSize,(5)* cellSize,cellSize,cellSize));
    nZ=countNonZero(cell);
    if(nZ > (cellSize*cellSize) /2){
        codeZ(4,0) = 1;
    }else{
        codeZ(4,0) = 0;
    }

    // d3
    cell = markerMat(Rect((3)* cellSize,(5)* cellSize,cellSize,cellSize));
    nZ=countNonZero(cell);
    if(nZ > (cellSize*cellSize) /2){
        codeZ(5,0) = 1;
    }else{
        codeZ(5,0) = 0;
    }

    // d4
    cell = markerMat(Rect((4)* cellSize,(5)* cellSize,cellSize,cellSize));
    nZ=countNonZero(cell);
    if(nZ > (cellSize*cellSize) /2){
        codeZ(6,0) = 1;
    }else{
        codeZ(6,0) = 0;
    }

    // calculate id
    int id = calculateId(codeX,codeY,codeZ);

    // get subpixel accuracy of marker position
    Size winSize = Size( 5, 5 );
    Size zeroZone = Size( -1, -1 );
    TermCriteria criteria = cvTermCriteria(CV_TERMCRIT_ITER ,30 ,0.01);
    cornerSubPix(m_grayMat, rectangle, winSize, zeroZone, criteria);

    // save marker...
    Marker finalMarker = Marker(rectangle.at(0),rectangle.at(1),rectangle.at(3),rectangle.at(2));
    finalMarker.setId(id);

    // get center point...
    //    Moments m = moments(rectangle,false);
    //    Point2f centerPoint = Point2f(m.m10/m.m00, m.m01/m.m00);

    float a1 = finalMarker.p3().x - finalMarker.p1().x;
    float b1 = finalMarker.p3().y - finalMarker.p1().y;
    float a2 = finalMarker.p4().x - finalMarker.p2().x;
    float b2 = finalMarker.p4().y - finalMarker.p2().y;

    float s = (a1*finalMarker.p2().y - a1*finalMarker.p1().y - b1*finalMarker.p2().x + b1*finalMarker.p1().x) / (b1*a2 - b2*a1);
    float t = (finalMarker.p2().x + s*a2 -finalMarker.p1().x)/(a1);

    Point2f centerPoint1 = finalMarker.p1() + t * (finalMarker.p3() - finalMarker.p1());
    Point2f centerPoint2 = finalMarker.p2() + s * (finalMarker.p4() - finalMarker.p2());

    // centerPoint1 and centerPoint2 have to be equal...otherwise there is no center
    // if we have a small error from floating overload...compensate it with average x- and y- value
    Point2f centerPoint;
    centerPoint.x = (centerPoint1.x + centerPoint2.x) / 2;
    centerPoint.y = (centerPoint1.y + centerPoint2.y) / 2;

    finalMarker.setCenter(centerPoint);

    // calculate 3D position
    QMatrix4x4 tMatrix = estimateMarkerPose(rotateMarkerPoints(finalMarker));
    finalMarker.setTransformationMatrix(tMatrix);

    finalMarker.setCoordinateSystemPoints(m_coordinateSystemPoints);

    // save marker with id...and rotate points for correct orientation
    m_markerList.push_back(finalMarker);

    return id;
}


Mat MarkerSearchEngine::rotate(Mat matrix)
{
    m_rotationCounter = 0;
    Mat out = matrix.clone();
    int cellSize = 15;
    // rotate maximal 4 times...
    for(int i = 0; i < 4; i++){
        // check the (2,2) cell...if its white, we have the right orientation, if not...rotate 90 deg and check again.
        Mat roi = out(Rect(cellSize,cellSize,cellSize,cellSize));
        int nZ=countNonZero(roi);
        if(nZ > (cellSize*cellSize) /2){
            // if more than the half white...we have our orientation.
            return out;
        }else{
            // rotate 90 deg
            m_rotationCounter++;
            int len = std::max(out.cols, out.rows);
            Point2f pt(len/2., len/2.);
            Mat r = getRotationMatrix2D(pt, 90, 1.0);
            warpAffine(out, out, r, Size(len, len));
        }
    }
    return out;
}

Marker MarkerSearchEngine::rotateMarkerPoints(Marker marker)
{
    Marker newMarker = marker;
    if(m_rotationCounter == 0){
        return marker;
    }
    for(int i = 0; i < m_rotationCounter; i++){
        newMarker.setP1(marker.p2());
        newMarker.setP2(marker.p3());
        newMarker.setP3(marker.p4());
        newMarker.setP4(marker.p1());
        marker = newMarker;
    }
    newMarker.setRotation(m_rotationCounter);
    return newMarker;
}

QGenericMatrix<1,7,int> MarkerSearchEngine::correctCode(QGenericMatrix<1,7,int> code)
{
    // parity check
    int hMatrixValues[7*3] = {1,0,1,0,1,0,1,
                              0,1,1,0,0,1,1,
                              0,0,0,1,1,1,1};

    QGenericMatrix<7,3,int> hMatrix(hMatrixValues);


    QGenericMatrix<1,3,int> syndrome = hMatrix * code;

    for(int i = 0; i < 3; i++){
        syndrome(i,0) = syndrome(i,0) %2;
    }

    // if the z vector is 0 -> pairity ok! no errors in transmission
    // TODO: correct error if there is one...
    //qDebug() << "parity check -> syndrome = " << syndrome;

    return code;
}

int MarkerSearchEngine::calculateId(QGenericMatrix<1, 7, int> codeX, QGenericMatrix<1, 7, int> codeY, QGenericMatrix<1, 7, int> codeZ)
{
    // [p1, p2, d1, p3, d2, d3, d4]^T
    QByteArray binCode;
    codeX = correctCode(codeX);
    binCode.append(QByteArray::number(codeX(2,0)));
    binCode.append(QByteArray::number(codeX(4,0)));
    binCode.append(QByteArray::number(codeX(5,0)));
    binCode.append(QByteArray::number(codeX(6,0)));

    codeY = correctCode(codeY);
    binCode.append(QByteArray::number(codeY(2,0)));
    binCode.append(QByteArray::number(codeY(4,0)));
    binCode.append(QByteArray::number(codeY(5,0)));
    binCode.append(QByteArray::number(codeY(6,0)));

    codeZ = correctCode(codeZ);
    binCode.append(QByteArray::number(codeZ(2,0)));
    binCode.append(QByteArray::number(codeZ(4,0)));
    binCode.append(QByteArray::number(codeZ(5,0)));
    binCode.append(QByteArray::number(codeZ(6,0)));

    //qDebug() << "Id = " << binCode << "=" << binCode.toInt(0,2);
    return binCode.toInt(0,2);
}

QMatrix4x4 MarkerSearchEngine::estimateMarkerPose(Marker marker)
{
    float markerSize = 50; // [mm]

    // set object points (virtual marker in the center of the image wit a side leght of markerSize)
    vector<Point3f> objectPoints;
    objectPoints.push_back(Point3f( -markerSize/2, -markerSize/2, 0));
    objectPoints.push_back(Point3f( -markerSize/2,  markerSize/2, 0));
    objectPoints.push_back(Point3f(  markerSize/2,  markerSize/2, 0));
    objectPoints.push_back(Point3f(  markerSize/2, -markerSize/2, 0));

    // set image points (detected marker corners)
    vector<Point2f> imagePoints;
    imagePoints.push_back(marker.p1());
    imagePoints.push_back(marker.p2());
    imagePoints.push_back(marker.p3());
    imagePoints.push_back(marker.p4());

    // set coordinate system points
    vector<Point3f> coordinateSytemPoints;
    coordinateSytemPoints.push_back(Point3f(markerSize/2, 0, 0)); // x-axis
    coordinateSytemPoints.push_back(Point3f(0, markerSize/2, 0)); // y-axis
    coordinateSytemPoints.push_back(Point3f(0, 0, markerSize/2)); // z-axis

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

    // calculate marker coodinate system points for dawing
    vector<Point2f> imageCoordinateSystemPoints;
    projectPoints(coordinateSytemPoints,rotationVector,translationVector,intrinsic,extrinsic,imageCoordinateSystemPoints);
    m_coordinateSystemPoints.clear();
    m_coordinateSystemPoints = imageCoordinateSystemPoints;

    return transformationMatrix;
}


