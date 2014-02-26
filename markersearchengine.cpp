#include "markersearchengine.h"

MarkerSearchEngine::MarkerSearchEngine(QObject *parent) :
    QObject(parent)
{
}

int MarkerSearchEngine::searchMarker(Mat &undistortMat)
{
    m_image = undistortMat;

    QList<QList<Point> > markers = findMarkerRectangles();

    if(markers.size() != 0){
        qDebug() << "found " << markers.size() << "markers";
    }

    return markers.size();
}

QList<QList<Point> > MarkerSearchEngine::findMarkerRectangles()
{

}

void MarkerSearchEngine::drawMarkers(Mat &image)
{

}

int MarkerSearchEngine::decodeMarkers()
{

}
