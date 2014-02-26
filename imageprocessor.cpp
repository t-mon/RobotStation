#include "imageprocessor.h"
#include "core.h"

ImageProcessor::ImageProcessor(QObject *parent) :
    QObject(parent)
{
    loadSettings();

    m_timer = new QTimer(this);
    m_timer->setInterval(30);

    connect(m_timer,SIGNAL(timeout()),this,SLOT(processImage()));
    m_timer->start();
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
    m_calibrated = false;

    // Threshold
    QSettings settings("masterarbeit");
    settings.beginGroup("general");
    //m_threshold = settings.value("threshold",100).toInt();
    settings.endGroup();

    // camera calibration parameter
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


    // TODO: switch process types

    emit imageReady(image);
    Core::instance()->window()->updateImage(convertMatToQimage(image));
}

void ImageProcessor::updateImage(const Mat &image)
{
    m_image = image;
}

void ImageProcessor::processTypeChanged(const int &processType)
{
    m_processType = (ProcessType)processType;

    switch (m_processType) {
    case OriginalImage:
        qDebug() << "process type changed to original image";
        break;
    case UndistortImage:
        qDebug() << "process type changed to undistort image";
        break;
    case GrayImage:
        qDebug() << "process type changed to gray image";
        break;
    case GrayEqualizedImage:
        qDebug() << "process type changed to equalized gray image";
        break;
    case GaussianBlurImage:
        qDebug() << "process type changed to gaussian blur image";
        break;
    case ThresholdImage:
        qDebug() << "process type changed to threshold image";
        break;
    case AdaptiveThresholdImage:
        qDebug() << "process type changed to adaptice threshold image";
        break;
    case CannyImage:
        qDebug() << "process type changed to canny image";
        break;
    case ResultImage:
        qDebug() << "process type changed to result image";
        break;
    default:
        break;
    }
}

void ImageProcessor::setFps(const int &fps)
{
    int dt = (int)( ((float)1 / (fps)) *1000 );
    qDebug() << "fps =" << fps << " -> dt =" << dt << "ms";
    m_timer->setInterval(dt);
}
