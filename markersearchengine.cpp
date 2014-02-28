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
    double fontScale = 0.8;
    int thickness = 1;

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
        circle(image,marker.center(),5,Scalar(0,0,255),1,8);

        // write id as text to center of marker
        string text = QString::number(marker.id()).toStdString();
        Point2f drawPoint = Point2f(marker.center().x+20,marker.center().y);
        putText(image, text, drawPoint, fontFace, fontScale, Scalar(0, 255, 0), thickness, 8);
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

    //imwrite("/home/timon/marker.jpg",markerMat);

    // check if inner squares are black or white
    Mat bitMatrix = Mat::zeros(5,5,CV_8UC1);
    for(int y = 0; y < 5; y++){
        for(int x = 0; x < 5; x++){
            int cellX = (x+1)*cellSize;
            int cellY = (y+1)*cellSize;
            Mat cell = markerMat(Rect(cellX,cellY,cellSize,cellSize));
            int nZ = countNonZero(cell);
            if(nZ > (cellSize*cellSize) / 2){
                bitMatrix.at<uchar>(y,x) = 1;
            }
        }
    }

    // check roations and calculate hamming distance for each rotation
    Mat rotations[4];
    int distances[4];

    rotations[0] = bitMatrix;
    distances[0] = hammDistMarker(rotations[0]);
    pair<int,int> minDist(distances[0],0);

    for(int i = 1; i < 4; i++){
        rotations[i] = rotate(rotations[i-1]);
        distances[i] = hammDistMarker(rotations[i]);
        if(distances[i] < minDist.first){
            minDist.first = distances[i];
            minDist.second = i;
        }
    }

    // if the smallest hamming distance is not 0
    if(minDist.first != 0){
        return -1;
    }

    // calculate id
    int id = 0;
    Mat bits = rotations[minDist.second];
    for(int y=0;y<5;y++){
        id <<=1;
        if ( bits.at<uchar>(y,1)) id|=1;
        id<<=1;
        if ( bits.at<uchar>(y,3)) id|=1;
    }

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

float MarkerSearchEngine::perimeter(vector<Point2f> rectangle)
{
    float sum=0;
    for(unsigned int i=0;i<rectangle.size();i++){
        int i2=(i+1)%rectangle.size();
        sum += sqrt((rectangle[i].x-rectangle[i2].x)*(rectangle[i].x-rectangle[i2].x)+(rectangle[i].y-rectangle[i2].y)*(rectangle[i].y-rectangle[i2].y)) ;
    }
    return sum;
}

int MarkerSearchEngine::hammDistMarker(Mat bits)
{
    int dist = 0;
    int ids[4][5] = {{1,0,0,0,0},{1,0,1,1,1},{0,1,0,0,1},{0, 1, 1, 1, 0}};

    for(int y=0;y<5;y++){
        int minSum=1e5;
        //hamming distance to each possible word
        for(int p=0;p<4;p++){
            int sum=0;
            for(int x=0;x<5;x++)
                sum+=  bits.at<uchar>(y,x) == ids[p][x] ? 0 : 1;
            if (minSum>sum) minSum=sum;
        }
        dist+=minSum;
    }
    return dist;
}

Mat MarkerSearchEngine::rotate(Mat matrix)
{
    Mat out = matrix.clone();
    for(int i = 0; i < matrix.rows; i++){
        for(int j = 0; j < matrix.cols; j++){
            out.at<uchar>(i, j)=matrix.at<uchar>(matrix.cols-j-1, i);
        }
    }
    return out;
}

