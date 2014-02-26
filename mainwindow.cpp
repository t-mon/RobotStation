#include "mainwindow.h"

#include <QDebug>
#include <QSettings>

#include "Tools/calibrationdialog.h"
#include "Tools/markergeneratordialog.h"
#include "Tools/snapshotdialog.h"

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

    QSettings settings("masterarbeit");

    // Image selection
    // ********************************************************************************

    QHBoxLayout *imageSelectionLayout = new QHBoxLayout(toolGroupBox);
    QLabel *imageSelectionLabel = new QLabel(tr("Show  "),toolGroupBox);
    m_imageComboBox = new QComboBox(toolGroupBox);
    m_imageComboBox->addItem("original image");
    m_imageComboBox->addItem("undistort image");
    m_imageComboBox->addItem("gray image");
    m_imageComboBox->addItem("gray equalized image");
    m_imageComboBox->addItem("blur image");
    m_imageComboBox->addItem("threshold image");
    m_imageComboBox->addItem("adaptive threshold image");
    m_imageComboBox->addItem("canny image");
    m_imageComboBox->addItem("result image");

    connect(m_imageComboBox,SIGNAL(activated(int)),this,SIGNAL(processTypeChanged(int)));

    imageSelectionLayout->addWidget(imageSelectionLabel);
    imageSelectionLayout->addWidget(m_imageComboBox);

    // Video Start Stop
    // ********************************************************************************
    QHBoxLayout *videoLayout = new QHBoxLayout(toolGroupBox);
    QPushButton *startVideo = new QPushButton("Start video");
    QPushButton *pauseVideo = new QPushButton("Pause");

    videoLayout->addWidget(startVideo);
    videoLayout->addWidget(pauseVideo);

    connect(startVideo,SIGNAL(clicked()),this,SIGNAL(startVideo()));
    connect(pauseVideo,SIGNAL(clicked()),this,SIGNAL(pauseVideo()));

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

    QLabel *thresholdLabel = new QLabel(tr("Threshold: "),toolGroupBox);
    m_thresholdValueLabel = new QLabel(settings.value("threshold",100).toString(),toolGroupBox);
    m_thresholdValueLabel->setFixedWidth(40);

    m_thresholdSlider = new QSlider(Qt::Horizontal,toolGroupBox);
    m_thresholdSlider->setMaximum(255);
    m_thresholdSlider->setMinimum(0);
    m_thresholdSlider->setTickInterval(1);
    m_thresholdSlider->setTickPosition(QSlider::TicksBothSides);
    m_thresholdSlider->setValue(settings.value("threshold",100).toInt());
    m_thresholdSlider->setMinimumWidth(200);

    connect(m_thresholdSlider,SIGNAL(valueChanged(int)),this,SLOT(updateThresholdValue(int)));

    thresholdLayout->addWidget(thresholdLabel);
    thresholdLayout->addWidget(m_thresholdSlider);
    thresholdLayout->addWidget(m_thresholdValueLabel);


    // ********************************************************************************
    toolLayout->addLayout(videoLayout);
    toolLayout->addWidget(takeSnapshotButton);
    toolLayout->addLayout(fpsLayout);
    toolLayout->addLayout(imageSelectionLayout);
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

    m_codeSearchAction = new QAction(tr("Search &code"),this);
    m_toolMenu->addAction(m_codeSearchAction);
    connect(m_codeSearchAction,SIGNAL(triggered()),this,SLOT(startCodeSearchDialog()));

    m_markerCreatorAction = new QAction(tr("Create ArUco Marker"),this);
    m_toolMenu->addAction(m_markerCreatorAction);
    connect(m_markerCreatorAction,SIGNAL(triggered()),this,SLOT(startMarkerGeneratorDialog()));
}

void MainWindow::startCalibrationDialog()
{
    CalibrationDialog calibrationDialog;
    calibrationDialog.exec();
}

void MainWindow::startCodeSearchDialog()
{

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

void MainWindow::updateThresholdValue(const int &threshold)
{
    QSettings settings("masterarbeit");
    settings.setValue("threshold",threshold);

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
