#include "imageprocessor.h"
#include "core.h"

ImageProcessor::ImageProcessor(QObject *parent) :
    QObject(parent)
{
    loadSettings();

    m_timer = new QTimer(this);
    m_timer->setInterval(30);

    m_markerSearchEngine = new MarkerSearchEngine(this);

    connect(m_timer,SIGNAL(timeout()),this,SLOT(processImage()));
}

Mat ImageProcessor::convertQImageToMat(QImage image)
{
    switch (image.format()){
    // 8-bit, 4 channel
    case QImage::Format_RGB32:{
        return Mat(image.height(), image.width(), CV_8UC4, const_cast<uchar*>(image.bits()), image.bytesPerLine());
    }
        // 8-bit, 3 channel
    case QImage::Format_RGB888:{
        QImage swapped = image.rgbSwapped();
        return Mat(swapped.height(), swapped.width(), CV_8UC3, const_cast<uchar*>(swapped.bits()), swapped.bytesPerLine());
    }
        // 8-bit, 1 channel
    case QImage::Format_Indexed8:{
        return Mat(image.height(), image.width(), CV_8UC1, const_cast<uchar*>(image.bits()), image.bytesPerLine());
    }
    default:
        Core::instance()->window()->writeToTerminal("ERROR: can not convert this image format");
        Core::instance()->window()->writeToTerminal("______________________________________________");
        break;
    }
    return Mat();
}

QImage ImageProcessor::convertMatToQimage(Mat imageMat)
{
    switch (imageMat.type()){
    // 8-bit, 4 channel
    case CV_8UC4:{
        QImage image(imageMat.data, imageMat.cols, imageMat.rows, imageMat.step, QImage::Format_RGB32);
        return image;
    }
        // 8-bit, 3 channel
    case CV_8UC3:{
        QImage image(imageMat.data, imageMat.cols, imageMat.rows, imageMat.step, QImage::Format_RGB888);
        return image.rgbSwapped();
    }
        // 8-bit, 1 channel
    case CV_8UC1:{
        QImage image(imageMat.data, imageMat.cols, imageMat.rows, imageMat.step, QImage::Format_Indexed8);
        return image;
    }
    default:
        Core::instance()->window()->writeToTerminal("ERROR: can not convert this image format");
        Core::instance()->window()->writeToTerminal("______________________________________________");
        break;
    }
    return QImage();
}

void ImageProcessor::saveCalibrationParameter(Mat intrinsic, Mat extrinsic)
{
    // calibration parameter
    //
    //              | fx  0   cx ]
    //  intrinsic = | 0   fy  cy ]    extrinsic = [ k1, k2, p1, p2, k3 ]
    //              | 0   0   1  ]

    double fx = intrinsic.at<double>(0,0);
    double fy = intrinsic.at<double>(1,1);
    double cx = intrinsic.at<double>(0,2);
    double cy = intrinsic.at<double>(1,2);

    QSettings settings("RobotStation");

    settings.beginGroup("intrinsic");
    settings.setValue("fx",fx);
    settings.setValue("fy",fy);
    settings.setValue("cx",cx);
    settings.setValue("cy",cy);
    settings.endGroup();

    qDebug() << "saving calibration parameter...";
    qDebug() << "----------------------------------------";

    qDebug() << "saving intrinsic parameter:";
    qDebug() << "   fx = " << fx;
    qDebug() << "   fy = " << fy;
    qDebug() << "   cx = " << cx;
    qDebug() << "   cy = " << cy;

    // [ k1, k2, p1, p2, k3 ]
    double k1 = extrinsic.at<double>(0,0);
    double k2 = extrinsic.at<double>(0,1);
    double p1 = extrinsic.at<double>(0,2);
    double p2 = extrinsic.at<double>(0,3);
    double k3 = extrinsic.at<double>(0,4);

    settings.beginGroup("extrinsic");
    settings.setValue("k1",k1);
    settings.setValue("k2",k2);
    settings.setValue("p1",p1);
    settings.setValue("p2",p2);
    settings.setValue("k3",k3);
    settings.endGroup();

    qDebug() << "saving extrinsic parameter:";
    qDebug() << "   k1 = " << k1;
    qDebug() << "   k2 = " << k2;
    qDebug() << "   p1 = " << p1;
    qDebug() << "   p2 = " << p2;
    qDebug() << "   k3 = " << k3;
    qDebug() << "----------------------------------------";

    // use from now on new parameter
    m_intrinsic = intrinsic;
    m_extrinsic = extrinsic;

    m_calibrated = true;

}

void ImageProcessor::loadSettings()
{
    qDebug() << "----------------------------------------";
    qDebug() << "loading settings";
    qDebug() << "----------------------------------------";

    // general settings
    QSettings settings("RobotStation");
    m_threshold = settings.value("threshold",100).toInt();
    qDebug() << "threshold = " << m_threshold;
    qDebug() << "----------------------------------------";

    m_processType = settings.value("processType",0).toInt();
    qDebug() << "processType = " << m_processType;
    qDebug() << "----------------------------------------";


    // camera calibration parameter
    // intrinsic
    settings.beginGroup("intrinsic");
    double fx = settings.value("fx",0).toDouble();
    double fy = settings.value("fy",0).toDouble();
    double cx = settings.value("cx",0).toDouble();
    double cy = settings.value("cy",0).toDouble();
    settings.endGroup();

    m_intrinsic = Mat::eye(Size(3,3),CV_64F);
    m_intrinsic.at<double>(0,0) = fx;
    m_intrinsic.at<double>(1,1) = fy;
    m_intrinsic.at<double>(0,2) = cx;
    m_intrinsic.at<double>(1,2) = cy;

    qDebug() << "intrinsic parameter:";
    qDebug() << "   fx = " << fx;
    qDebug() << "   fy = " << fy;
    qDebug() << "   cx = " << cx;
    qDebug() << "   cy = " << cy;
    qDebug() << "----------------------------------------";

    bool intrinsicFound = false;
    if(fx == 0 && fy == 0 && cx == 0 && cy == 0){
        qDebug() << "---> no intrinsic calibration parameter found.";
        intrinsicFound = false;
    }else{
        qDebug() << "---> intrinsic parameter found.";
        intrinsicFound = true;
    }
    qDebug() << "----------------------------------------";


    // extrinsic
    settings.beginGroup("extrinsic");
    double k1 = settings.value("k1",0).toDouble();
    double k2 = settings.value("k2",0).toDouble();
    double p1 = settings.value("p1",0).toDouble();
    double p2 = settings.value("p2",0).toDouble();
    double k3 = settings.value("k3",0).toDouble();
    settings.endGroup();

    m_extrinsic = Mat::zeros(1,5,CV_64F);
    m_extrinsic.at<double>(0,0) = k1;
    m_extrinsic.at<double>(0,1) = k2;
    m_extrinsic.at<double>(0,2) = p1;
    m_extrinsic.at<double>(0,3) = p2;
    m_extrinsic.at<double>(0,4) = k3;

    qDebug() << "extrinsic parameter:";
    qDebug() << "   k1 = " << k1;
    qDebug() << "   k2 = " << k2;
    qDebug() << "   p1 = " << p1;
    qDebug() << "   p2 = " << p2;
    qDebug() << "   k3 = " << k3;
    qDebug() << "----------------------------------------";

    bool extrinsicFound = false;
    if(k1 == 0 && k2 == 0 && p1 == 0 && p2 == 0 && k3 == 0){
        qDebug() << "---> no extrinsic calibration parameter found.";
        extrinsicFound = false;
    }else{
        qDebug() << "---> extrinsic parameter found.";
        extrinsicFound = true;
    }
    qDebug() << "----------------------------------------";
    if(intrinsicFound && extrinsicFound){
        m_calibrated = true;
    }else{
        m_calibrated = false;
    }
}

void ImageProcessor::processImage()
{
    if(m_image.empty()){
        return;
    }

    Mat image;
    if(m_calibrated){
        undistort(m_image,image,m_intrinsic,m_extrinsic);
    }else{
        image = m_image;
    }

    emit imageReady(image);
    emit originalImageReady(m_image);

    switch (m_processType) {
    case 0:{
        image = m_image;
        break;
    }
    case 1:
        break;
    case 2:{
        cvtColor(image, image, CV_BGR2GRAY);
        break;
    }
    case 3:{
        cvtColor(image, image, CV_BGR2GRAY);
        equalizeHist(image,image);
        break;
    }
    case 4:{
        cvtColor(image, image, CV_BGR2GRAY);
        threshold(image,image,m_threshold,255,CV_THRESH_BINARY);
        break;
    }
    case 5:{
        cvtColor(image, image, CV_BGR2GRAY);
        adaptiveThreshold(image, image, 255, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY_INV, 7, 7);
        break;
    }
    case 6:{
        cvtColor(image, image, CV_BGR2GRAY);
        //adaptiveThreshold(image, image, 255, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY_INV, 7, 7);
        Canny(image,image,m_threshold,m_threshold+50);
        break;
    }
    case 7:{
        QList<Marker> markerList = m_markerSearchEngine->searchMarker(image);
        m_markerSearchEngine->drawMarkers(image,markerList);
        break;
    }
    default:
        break;
    }

    Core::instance()->window()->updateImage(convertMatToQimage(image));
}

void ImageProcessor::updateImage(const Mat &image)
{
    m_image = image;
}

void ImageProcessor::processTypeChanged(const int &processType)
{

    m_processType = processType;

    QSettings settings("RobotStation");
    settings.setValue("processType",processType);

    switch (m_processType) {
    case 0:
        qDebug() << "process type changed to original image";
        break;
    case 1:
        qDebug() << "process type changed to undistort image";
        break;
    case 2:
        qDebug() << "process type changed to gray image";
        break;
    case 3:
        qDebug() << "process type changed to equalized gray image";
        break;
    case 4:
        qDebug() << "process type changed to threshold image";
        break;
    case 5:
        qDebug() << "process type changed to adaptice threshold image";
        break;
    case 6:
        qDebug() << "process type changed to canny image";
        break;
    case 7:
        qDebug() << "process type changed to result image";
        break;
    default:
        break;
    }
}

void ImageProcessor::thresholdValueChanged(const int &threshold)
{
    m_threshold = threshold;
    QSettings settings("RobotStation");
    settings.setValue("threshold",threshold);
}

void ImageProcessor::setFps(const int &fps)
{
    int dt = (int)( ((float)1 / (fps)) *1000 );
    qDebug() << "fps =" << fps << " -> dt =" << dt << "ms";
    m_timer->setInterval(dt);
}

void ImageProcessor::startProcessor()
{
    m_timer->start();
    qDebug() << "image processor started.";
}

void ImageProcessor::stopProcessor()
{
    m_timer->stop();
    qDebug() << "image processor stopped.";
}
