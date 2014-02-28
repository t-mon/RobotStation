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

    QString filename;
    filename.append(QDir::homePath());
    filename.append("/.config/calibrationParameter.xml");
    Core::instance()->window()->writeToTerminal("saving calibration parameter... " + filename);
    Core::instance()->window()->writeToTerminal("______________________________________________");

    FileStorage fs;
    if(fs.open(filename.toStdString(), FileStorage::WRITE)){
        fs << "CameraMatrix" << intrinsic;
        fs << "DistCoeffs" << extrinsic;
        fs.release();
    }else{
        Core::instance()->window()->writeToTerminal("ERROR: could not save calibration parameter... " + filename);
        Core::instance()->window()->writeToTerminal("______________________________________________");
    }

    // use from now on new parameter
    m_intrinsic = intrinsic;
    m_extrinsic = extrinsic;

    m_calibrated = true;
}

void ImageProcessor::loadSettings()
{

    // general settings
    QSettings settings("RobotStation");
    m_threshold = settings.value("threshold",100).toInt();
    m_processType = settings.value("processType",0).toInt();

    // camera calibration parameter
    m_calibrated = false;

    QString filename;
    filename.append(QDir::homePath());
    filename.append("/.config/calibrationParameter.xml");
    qDebug() << "load settings..." << filename;
    qDebug() << "______________________________________________";

    FileStorage fs;
    if(fs.open(filename.toStdString(), FileStorage::READ)){
        fs["CameraMatrix"] >> m_intrinsic;
        fs["DistCoeffs"] >> m_extrinsic;
        fs.release();
        m_calibrated = true;
    }else{
        qDebug() << "ERROR: no calibration data found";
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
    case 0:
        image = m_image;
        break;
    case 1:
        break;
    case 2:
        cvtColor(image, image, CV_BGR2GRAY);
        break;
    case 3:
        cvtColor(image, image, CV_BGR2GRAY);
        equalizeHist(image,image);
        break;
    case 4:
        cvtColor(image, image, CV_BGR2GRAY);
        threshold(image,image,m_threshold,255,CV_THRESH_BINARY);
        break;
    case 5:
        cvtColor(image, image, CV_BGR2GRAY);
        adaptiveThreshold(image, image, 255, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY_INV, 7, 7);
        break;
    case 6:
        cvtColor(image, image, CV_BGR2GRAY);
        //adaptiveThreshold(image, image, 255, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY_INV, 7, 7);
        Canny(image,image,m_threshold,m_threshold+50);
        break;
    case 7:
        m_markerSearchEngine->searchMarker(image);
        break;
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
