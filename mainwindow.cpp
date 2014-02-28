#include "mainwindow.h"

#include <QDebug>
#include <QSettings>

#include "Tools/calibrationdialog.h"
#include "Tools/markergeneratordialog.h"
#include "Tools/snapshotdialog.h"
#include "Tools/qrcodesearchdialog.h"
#include "Tools/markersearchdialog.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    m_centralWidget = new QWidget(this);
    setCentralWidget(m_centralWidget);

    setWindowTitle("Robot Station");
    setMinimumHeight(600);
    createMenus();

    QHBoxLayout *mainLayout = new QHBoxLayout(m_centralWidget);
    m_centralWidget->setLayout(mainLayout);

    QVBoxLayout *toolsLayout = new QVBoxLayout();

    m_statusBar = new QStatusBar(this);
    setStatusBar(m_statusBar);

    toolsLayout->addWidget(createToolGroupBox());
    toolsLayout->addWidget(createTerminalGroupBox());

    mainLayout->addWidget(createImageGroupBox(),Qt::AlignCenter);
    mainLayout->addLayout(toolsLayout);

}

MainWindow::~MainWindow()
{

}

QGroupBox *MainWindow::createImageGroupBox()
{
    QGroupBox *imageGroupBox = new QGroupBox(tr("Image"),m_centralWidget);
    QVBoxLayout *imageLayout = new QVBoxLayout(imageGroupBox);

    m_imageLabel = new QLabel(imageGroupBox);
    m_imageLabel->setAutoFillBackground(true);
    m_imageLabel->showMaximized();

    imageLayout->addWidget(m_imageLabel,Qt::AlignCenter);

    imageGroupBox->setFixedWidth(640);
    imageGroupBox->setLayout(imageLayout);
    return imageGroupBox;
}

QGroupBox *MainWindow::createToolGroupBox()
{
    QGroupBox *toolGroupBox = new QGroupBox(tr("Tools"),m_centralWidget);
    QVBoxLayout *toolLayout = new QVBoxLayout(toolGroupBox);
    toolGroupBox->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);


    // process selection
    // ********************************************************************************
    QSettings settings("RobotStation");
    int processType = settings.value("processType",0).toInt();

    QHBoxLayout *processLayout = new QHBoxLayout(toolGroupBox);
    QLabel *processLabel = new QLabel(tr("Select image process"),toolGroupBox);
    m_imageComboBox = new QComboBox(toolGroupBox);
    m_imageComboBox->addItem("original image",QVariant(0));
    m_imageComboBox->addItem("undistort image",QVariant(1));
    m_imageComboBox->addItem("gray image",QVariant(2));
    m_imageComboBox->addItem("gray equalized image",QVariant(3));
    m_imageComboBox->addItem("threshold image",QVariant(4));
    m_imageComboBox->addItem("adaptive threshold image",QVariant(5));
    m_imageComboBox->addItem("canny image",QVariant(6));
    m_imageComboBox->addItem("result image",QVariant(7));

    m_imageComboBox->setCurrentIndex(processType);

    connect(m_imageComboBox,SIGNAL(activated(int)),this,SIGNAL(processTypeChanged(int)));

    processLayout->addWidget(processLabel);
    processLayout->addWidget(m_imageComboBox);

    // Video Start Stop
    // ********************************************************************************
    QHBoxLayout *videoLayout = new QHBoxLayout(toolGroupBox);
    QPushButton *startVideo = new QPushButton("Start video");
    QPushButton *stopVideo = new QPushButton("Stop video");

    videoLayout->addWidget(startVideo);
    videoLayout->addWidget(stopVideo);

    connect(startVideo,SIGNAL(clicked()),this,SIGNAL(startVideo()));
    connect(stopVideo,SIGNAL(clicked()),this,SIGNAL(stopVideo()));

    // Take Snapshot
    // ********************************************************************************
    QPushButton *takeSnapshotButton = new QPushButton("Take snapshot");
    connect(takeSnapshotButton,SIGNAL(clicked()),this,SLOT(startSnapshotDialog()));

    // Set Fps
    // ********************************************************************************
    QHBoxLayout *fpsLayout = new QHBoxLayout(toolGroupBox);
    QLabel *fpsDescriptionLabel = new QLabel(tr("fps"),toolGroupBox);
    m_fpsLabel = new QLabel("20",toolGroupBox);
    m_fpsLabel->setFixedWidth(40);

    m_fpsSlider = new QSlider(Qt::Horizontal,toolGroupBox);
    m_fpsSlider->setMinimum(1);
    m_fpsSlider->setMaximum(30);
    m_fpsSlider->setTickInterval(1);
    m_fpsSlider->setTickPosition(QSlider::TicksBothSides);
    m_fpsSlider->setValue(20);

    connect(m_fpsSlider,SIGNAL(valueChanged(int)),this,SLOT(updateFpsValue(int)));

    fpsLayout->addWidget(fpsDescriptionLabel);
    fpsLayout->addWidget(m_fpsSlider);
    fpsLayout->addWidget(m_fpsLabel);

    // Threshold
    // ********************************************************************************
    QHBoxLayout *thresholdLayout = new QHBoxLayout(toolGroupBox);
    int threshold = settings.value("threshold",100).toInt();

    QLabel *thresholdLabel = new QLabel(tr("Threshold: "),toolGroupBox);
    m_thresholdValueLabel = new QLabel(QString::number(threshold),toolGroupBox);
    m_thresholdValueLabel->setFixedWidth(40);

    m_thresholdSlider = new QSlider(Qt::Horizontal,toolGroupBox);
    m_thresholdSlider->setMaximum(255);
    m_thresholdSlider->setMinimum(0);
    m_thresholdSlider->setTickInterval(1);
    m_thresholdSlider->setTickPosition(QSlider::TicksBothSides);
    m_thresholdSlider->setMinimumWidth(200);
    m_thresholdSlider->setValue(threshold);

    connect(m_thresholdSlider,SIGNAL(valueChanged(int)),this,SLOT(updateThresholdValue(int)));

    thresholdLayout->addWidget(thresholdLabel);
    thresholdLayout->addWidget(m_thresholdSlider);
    thresholdLayout->addWidget(m_thresholdValueLabel);


    // ********************************************************************************
    toolLayout->addLayout(videoLayout);
    toolLayout->addWidget(takeSnapshotButton);
    toolLayout->addLayout(fpsLayout);
    toolLayout->addLayout(processLayout);
    toolLayout->addLayout(thresholdLayout);

    toolGroupBox->setLayout(toolLayout);

    return toolGroupBox;
}

QGroupBox *MainWindow::createTerminalGroupBox()
{
    QGroupBox *terminalGroupBox = new QGroupBox(tr("Terminal"),m_centralWidget);
    QVBoxLayout *terminalLayout = new QVBoxLayout(terminalGroupBox);

    m_terminal = new QTextEdit(terminalGroupBox);
    m_terminal->setReadOnly(true);

    terminalLayout->addWidget(m_terminal);
    terminalGroupBox->setLayout(terminalLayout);
    terminalGroupBox->setFixedHeight(250);

    return terminalGroupBox;
}

void MainWindow::createMenus()
{
    m_fileMenu = menuBar()->addMenu(tr("&File"));

    m_exitAction = new QAction(tr("E&xit"),this);
    m_fileMenu->addAction(m_exitAction);
    connect(m_exitAction,SIGNAL(triggered()),this,SLOT(close()));

    m_toolMenu = menuBar()->addMenu("&Tools");

    m_calibrateAction = new QAction(tr("&Calibrate camera"),this);
    m_toolMenu->addAction(m_calibrateAction);
    connect(m_calibrateAction,SIGNAL(triggered()),this,SLOT(startCalibrationDialog()));

    m_markerCreatorAction = new QAction(tr("Marker generator"),this);
    m_toolMenu->addAction(m_markerCreatorAction);
    connect(m_markerCreatorAction,SIGNAL(triggered()),this,SLOT(startMarkerGeneratorDialog()));

    m_markerSearchAction = new QAction(tr("Marker search engine"),this);
    m_toolMenu->addAction(m_markerSearchAction);
    connect(m_markerSearchAction,SIGNAL(triggered()),this,SLOT(startMarkerSearchDialog()));

    m_toolMenu->addSeparator();

    m_qrCodeSearchAction = new QAction(tr("QR-code search engine"),this);
    m_toolMenu->addAction(m_qrCodeSearchAction);
    connect(m_qrCodeSearchAction,SIGNAL(triggered()),this,SLOT(startCodeSearchDialog()));
}

void MainWindow::startCalibrationDialog()
{
    CalibrationDialog calibrationDialog;
    calibrationDialog.exec();
}

void MainWindow::startCodeSearchDialog()
{
    QRCodeSearchDialog qrCodeSearchDialog;
    qrCodeSearchDialog.exec();
}

void MainWindow::startSnapshotDialog()
{
    SnapshotDialog snapshotDialog(0,m_image);
    snapshotDialog.exec();
}

void MainWindow::startMarkerGeneratorDialog()
{
    MarkerGeneratorDialog markerGenerator;
    markerGenerator.exec();
}

void MainWindow::startMarkerSearchDialog()
{
    MarkerSearchDialog markerSearch;
    markerSearch.exec();
}

void MainWindow::updateThresholdValue(const int &threshold)
{
    m_thresholdValueLabel->setText(QString::number(threshold));
    emit thresholdValueChanged(threshold);
}

void MainWindow::updateFpsValue(const int &fps)
{
    m_fpsLabel->setText(QString::number(fps));
    emit fpsChanged(fps);
}

void MainWindow::writeToTerminal(const QString &message)
{
    qDebug() << message;
    m_terminal->setTextColor(QColor(0, 255, 0,255));
    m_terminal->append(message);
}

void MainWindow::updateImage(const QImage &image)
{
    m_image = image;

    int w = m_imageLabel->width();
    int h = m_imageLabel->height();

    QPixmap pixmap = QPixmap::fromImage(image);
    if(!pixmap.isNull()){
        m_imageLabel->setPixmap(pixmap.scaled(w,h,Qt::KeepAspectRatio));
    }
}
