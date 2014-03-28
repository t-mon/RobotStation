#include "calibrationdialog.h"
#include "core.h"

CalibrationDialog::CalibrationDialog(QWidget *parent) :
    QDialog(parent)
{
    // calibration stuff...
    init();

    //GUI stuff...
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    setWindowTitle("Camera calibration...");

    m_calibrationImageLabel = new QLabel(this);
    m_calibrationImageLabel->setAutoFillBackground(true);
    m_calibrationImageLabel->setFixedSize(640,480);

    m_statusText = new QLabel(this);
    m_statusText->setText("Collecting 30 cessboard images for calibration...\n\n\n");

    m_progressBar = new QProgressBar(this);

    QHBoxLayout *frameStepLayout = new QHBoxLayout(this);
    m_frameStepDialog = new QLabel("fps = 1",this);

    m_fpsSlider = new QSlider(Qt::Horizontal,this);
    m_fpsSlider->setMinimum(1);
    m_fpsSlider->setMaximum(30);
    m_fpsSlider->setTickInterval(1);
    m_fpsSlider->setValue(1);
    m_fpsSlider->setTickPosition(QSlider::TicksBothSides);
    Core::instance()->imageProcessor()->setFps(1);

    connect(m_fpsSlider,SIGNAL(valueChanged(int)),this,SLOT(fpsChaged(int)));

    frameStepLayout->addWidget(m_frameStepDialog);
    frameStepLayout->addWidget(m_fpsSlider);

    m_progressBar->setRange(0,m_calibrationImages);
    m_progressBar->setValue(m_progress);
    m_progressBar->setOrientation(Qt::Horizontal);

    QHBoxLayout *buttonLayout = new QHBoxLayout(this);
    m_saveButton = new QPushButton("Save");
    m_saveButton->setFlat(true);
    m_saveButton->setAutoFillBackground(true);
    m_saveButton->setDisabled(true);
    m_retryButton = new QPushButton("Retry");
    m_cancelButton = new QPushButton("Cancel");
    buttonLayout->addWidget(m_saveButton);
    buttonLayout->addWidget(m_retryButton);
    buttonLayout->addWidget(m_cancelButton);

    mainLayout->addWidget(m_calibrationImageLabel);
    mainLayout->addLayout(frameStepLayout);
    mainLayout->addWidget(m_statusText);
    mainLayout->addWidget(m_progressBar);
    mainLayout->addLayout(buttonLayout);

    connect(m_saveButton, SIGNAL(clicked()),this, SLOT(saveClicked()));
    connect(m_retryButton,SIGNAL(clicked()),this,SLOT(retryClicked()));
    connect(m_cancelButton, SIGNAL(clicked()),this, SLOT(cancelClicked()));

    connect(Core::instance()->imageProcessor(), SIGNAL(originalImageReady(Mat)), this, SLOT(updateImage(Mat)));

}

void CalibrationDialog::init()
{
    m_calibrationImages = 30;

    m_sideLenght = 25; // [mm]

    m_patternSize       = Size(9,6);
    m_cornersNumber     = m_patternSize.height * m_patternSize.height;

    // Calculate the object points in the object coordinate system (origin at top left corner)
    for(int x = 0; x < m_calibrationImages; x++){
        vector<Point3f> objectPoints;
        for(int i = 0; i < m_patternSize.height; i++) {
            for(int j = 0; j < m_patternSize.width; j++) {
                objectPoints.push_back(Point3f(j * m_sideLenght, i * m_sideLenght, 0.f));
            }
        }
        m_objectPoints.push_back(objectPoints);
    }

    m_progress = 0;
}

void CalibrationDialog::fpsChaged(const int &fps)
{
    m_frameStepDialog->setText("fps = " + QString::number(fps));
    Core::instance()->imageProcessor()->setFps(fps);
}

void CalibrationDialog::saveClicked()
{
    Core::instance()->imageProcessor()->saveCalibrationParameter(m_intrinsic,m_extrinsic);
    accept();
}

void CalibrationDialog::retryClicked()
{
    m_statusText->setText("Collecting cessboard images for calibration...\n\n\n");

    m_intrinsic = Mat::zeros(m_intrinsic.size(),m_intrinsic.type());
    m_extrinsic = Mat::zeros(m_extrinsic.size(),m_extrinsic.type());

    m_progress = 0;
    m_progressBar->setValue(0);

    m_fpsSlider->setValue(2);

    m_saveButton->setFlat(true);
    m_saveButton->setDisabled(true);

    m_imagePoints.clear();
}

void CalibrationDialog::cancelClicked()
{
    Core::instance()->imageProcessor()->setFps(20);
    reject();
}

void CalibrationDialog::updateImage(const Mat &image)
{
    // if we have not yet collected all images...
    if(m_progress != m_calibrationImages){
        //search for cessboard
        std::vector<Point2f> corners;

        Mat grayImage;
        cvtColor(image,grayImage,CV_BGR2GRAY);
        bool found = findChessboardCorners(grayImage, m_patternSize, corners, CALIB_CB_ADAPTIVE_THRESH + CALIB_CB_NORMALIZE_IMAGE + CALIB_CB_FAST_CHECK);

        if(found){
            // found a cessboard
            m_progress++;
            m_progressBar->setValue(m_progress);

            // Get subpixel accuracy on corners
            cornerSubPix(grayImage, corners, Size(11,11), Size(-1,-1), TermCriteria(CV_TERMCRIT_EPS+CV_TERMCRIT_ITER, 30, 0.1));

            // save the found corners
            m_imagePoints.push_back(corners);

            drawChessboardCorners(image,m_patternSize, corners, found);
        }

        // if we have collected all cessboardimages...
        if(m_progress == m_calibrationImages){

            m_statusText->setText("Calibrating...\n\n\n");

            // calculate parameters
            vector<Mat> rvecs;
            vector<Mat> tvecs;

            double rmsError = calibrateCamera(m_objectPoints, m_imagePoints, image.size(), m_intrinsic, m_extrinsic, rvecs, tvecs,CV_CALIB_FIX_K3);
            m_statusText->setText("Calibration done. \nRMS reprojection error = " + QString::number(rmsError) + " Pixel (should be < 0,5 Pixel)\nShowing undistort camera image...\nClick \"save\" to use this parameters from now on in this application.");

            qDebug() << "Calibration done...";
            qDebug() << "RMS error" << rmsError;
            qDebug() << "______________________________________________";

            m_fpsSlider->setValue(20);

            // offer save parameters
            m_saveButton->setFlat(false);
            m_saveButton->setEnabled(true);
        }


        // show snapshot
        int w = m_calibrationImageLabel->width();
        int h = m_calibrationImageLabel->height();

        QPixmap pixmap = QPixmap::fromImage(Core::instance()->imageProcessor()->convertMatToQimage(image));
        if(!pixmap.isNull()){
            m_calibrationImageLabel->setPixmap(pixmap.scaled(w,h,Qt::KeepAspectRatio));
        }

    }else{
        // show remaped image
        Mat undistortMap;
        undistort(image,undistortMap,m_intrinsic,m_extrinsic);

        // show remaped image
        int w = m_calibrationImageLabel->width();
        int h = m_calibrationImageLabel->height();

        QImage img = Core::instance()->imageProcessor()->convertMatToQimage(undistortMap);
        QPixmap pixmap = QPixmap::fromImage(img);
        if(!pixmap.isNull()){
            m_calibrationImageLabel->setPixmap(pixmap.scaled(w,h,Qt::KeepAspectRatio));
        }
    }
}
