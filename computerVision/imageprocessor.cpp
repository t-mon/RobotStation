#include "imageprocessor.h"
#include "core.h"

ImageProcessor::ImageProcessor(QObject *parent) :
    QObject(parent)
{
    loadSettings();

    m_timer = new QTimer(this);
    m_timer->setInterval(100);
    m_poseEngine = new PoseEstimationEngine(this);

    connect(m_timer,SIGNAL(timeout()),this,SLOT(updateImage()));
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
        Core::instance()->window()->writeErrorToTerminal("ERROR: can not convert this image format");
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
        Core::instance()->window()->writeErrorToTerminal("ERROR: can not convert this image format");
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

    qDebug() << "saving calibration parameter to" << settings.fileName();
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

    Core::instance()->window()->writeToTerminal("calibration parameter saved to " + settings.fileName());
}

Mat ImageProcessor::getIntrinsic()
{
    if(m_calibrated){
        return m_intrinsic;
    }else{
        return Mat::zeros(Size(3,3),CV_64F);
    }
}

Mat ImageProcessor::getExtrinsic()
{
    if(m_calibrated){
        return m_extrinsic;
    }else{
        return Mat::zeros(1,5,CV_64F);
    }
}

QImage ImageProcessor::image()
{
    return convertMatToQimage(m_image);
}

void ImageProcessor::loadSettings()
{

    // general settings
    QSettings settings("RobotStation");

    qDebug() << "----------------------------------------";
    qDebug() << "loading settings" << settings.fileName();
    qDebug() << "----------------------------------------";

    m_processType = settings.value("processType",0).toInt();
    qDebug() << "processType = " << m_processType;
    qDebug() << "----------------------------------------";

    m_alpha = settings.value("contrast",1).toDouble();
    qDebug() << "contrast = " << m_alpha;
    qDebug() << "----------------------------------------";

    m_beta = settings.value("brightness",0).toDouble();
    qDebug() << "brightness = " << m_beta;
    qDebug() << "----------------------------------------";

    // camera calibration parameter
    // intrinsic
    settings.beginGroup("intrinsic");
    double fx = settings.value("fx",0).toDouble();
    double fy = settings.value("fy",0).toDouble();
    double cx = settings.value("cx",0).toDouble();
    double cy = settings.value("cy",0).toDouble();
    settings.endGroup();

    m_intrinsic = Mat::zeros(Size(3,3),CV_64F);
    m_intrinsic.at<double>(0,0) = fx;
    m_intrinsic.at<double>(1,1) = fy;
    m_intrinsic.at<double>(0,2) = cx;
    m_intrinsic.at<double>(1,2) = cy;
    m_intrinsic.at<double>(2,2) = 1;

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

    Core::instance()->window()->writeToTerminal("settings loaded..." + settings.fileName());
}

void ImageProcessor::processImage()
{
    if(m_image.empty()){
        return;
    }

    emit originalImageReady(m_image);

    // check if we have a calibration...
    Mat image;
    if(m_calibrated){
        // if we have one use the calibrated image for all other processes
        undistort(m_image,image,m_intrinsic,m_extrinsic);
    }else{
        // else use just the original image
        image = m_image;
    }

    // adjust brightness and contrast
    image.convertTo(image,-1,m_alpha,m_beta);
    emit imageReady(image);


    // show image to gui...depending on processtype...
    // switching between process types
    switch (m_processType) {

    case 0:{
        // original
        image = m_image;
        break;
    }
    case 1:{
        // result image
        m_poseEngine->updateImage(image);
        break;
    }
    default:
        break;
    }
    QImage imageToShow = convertMatToQimage(image);
    Core::instance()->window()->updateImage(imageToShow.mirrored(false,false));
}

void ImageProcessor::updateImage()
{
    m_image = Core::instance()->cameraEngine()->image();
    if(!m_image.empty()){
        processImage();
    }
}

void ImageProcessor::updateProcessType(const int &processType)
{

    m_processType = processType;

    QSettings settings("RobotStation");
    settings.setValue("processType",processType);

    switch (m_processType) {
    case 0:
        qDebug() << "process type changed to original image";
        break;
    case 1:
        qDebug() << "process type changed to result image";
        break;
    default:
        break;
    }
}

void ImageProcessor::updateBrightness(const double &brightness)
{
    m_beta = brightness;
    qDebug() << "brightness =" << brightness;
    QSettings settings("RobotStation");
    settings.setValue("brightness",brightness);
}

void ImageProcessor::updateContrast(const double &contrast)
{
    m_alpha = contrast;
    qDebug() << "contrast =" << contrast;
    QSettings settings("RobotStation");
    settings.setValue("contrast",contrast);
}

void ImageProcessor::updateFps(const int &fps)
{
    int dt = (int)( ((float)1 / (fps)) *1000 );
    qDebug() << "fps =" << fps << " -> dt =" << dt << "ms";
    m_timer->setInterval(dt);
}

void ImageProcessor::startProcessor()
{
    m_timer->start();
    Core::instance()->window()->writeToTerminal("image processor started.");
}

void ImageProcessor::stopProcessor()
{
    m_timer->stop();
    Core::instance()->window()->writeToTerminal("image processor stopped.");
}

