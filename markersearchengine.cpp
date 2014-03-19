#include "markersearchengine.h"

MarkerSearchEngine::MarkerSearchEngine(QObject *parent) :
    QObject(parent)
{

}

QList<Marker> MarkerSearchEngine::searchMarker(Mat &undistortMat)
{
    // clean up old data
    m_rectangleList.clear();
    m_markerList.clear();

    cvtColor(undistortMat,m_grayMat,CV_BGR2GRAY);

    // first find all rectangles
    findRectangles();

    // then take try to decode the rectangle to check if its a marker
    foreach (const vector<Point2f> rectangle, m_rectangleList){
        decodeMarker(rectangle);
    }

    if(m_markerList.size() != 0){
        qDebug() << "found" << m_markerList.size() << "marker";
        foreach (const Marker &marker, m_markerList){
            qDebug() << "id: " << marker.id();
            //            qDebug() << "   p1 = (" << marker.p1().x << "," << marker.p1().y << ")";
            //            qDebug() << "   p2 = (" << marker.p2().x << "," << marker.p2().y << ")";
            //            qDebug() << "   p3 = (" << marker.p3().x << "," << marker.p3().y << ")";
            //            qDebug() << "   p4 = (" << marker.p4().x << "," << marker.p4().y << ")";
        }
        qDebug() << "-----------------------------------------";
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

        // write id as text to center of marker
        string text = QString::number(marker.id()).toStdString();
        Point2f drawPoint = Point2f(marker.center().x+20,marker.center().y);
        putText(image, text, drawPoint, fontFace, fontScale, Scalar(0, 255, 255), thickness, 8);
    }
}

void MarkerSearchEngine::findRectangles()
{
    // Adaptive threshold
    Mat tmp;
    adaptiveThreshold(m_grayMat, tmp, 255,ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY_INV, 7, 7);

    // find contours
    vector<vector<Point> > allContours;
    vector<Vec4i> hierarchy;
    findContours(tmp, allContours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);

    // filter small contours away
    int minContourSizeAllowed = 15;
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


    //Rotate in readable orientation
    markerMat = rotate(markerMat);
    //imwrite("/home/timon/marker.jpg",markerMat);



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

    //qDebug() << "data z =" << codeZ;

    // calculate id
    int id = calculateId(codeX,codeY,codeZ);

    // get subpixel accuracy of marker position
    Size winSize = Size( 5, 5 );
    Size zeroZone = Size( -1, -1 );
    TermCriteria criteria = cvTermCriteria(CV_TERMCRIT_ITER ,30 ,0.01);
    cornerSubPix(m_grayMat, rectangle, winSize, zeroZone, criteria);

    // get center point...
    Moments m = moments(rectangle,false);
    Point2f centerPoint = Point2f(m.m10/m.m00, m.m01/m.m00);

    // save marker...
    Marker finalMarker = Marker(rectangle.at(0),rectangle.at(1),rectangle.at(3),rectangle.at(2));
    finalMarker.setId(id);
    finalMarker.setCenter(centerPoint);

    // save marker with id...
    m_markerList.push_back(finalMarker);
    return id;
}


Mat MarkerSearchEngine::rotate(Mat matrix)
{
    Mat out = matrix.clone();
    int cellSize = 15;
    // rotate maximal 4 times...
    for(int i = 0; i < 4; i++){
        // check the 2,2 cell...if its white, we have the right orientation, if not...rotate 90 deg and check again.
        Mat roi = out(Rect(cellSize,cellSize,cellSize,cellSize));
        int nZ=countNonZero(roi);
        if(nZ > (cellSize*cellSize) /2){
            // if more than the half white...we have our orientation.
            return out;
        }else{
            // rotate 90 deg
            int len = std::max(out.cols, out.rows);
            Point2f pt(len/2., len/2.);
            Mat r = getRotationMatrix2D(pt, 90, 1.0);
            warpAffine(out, out, r, Size(len, len));

        }

    }
    return out;
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


