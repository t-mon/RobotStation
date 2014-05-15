/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  Copyright 2014 Simon Stuerz                                            *
 *  This file is part of RobotStation.                                     *
 *                                                                         *
 *  RobotStation is free software: you can redistribute it and/or modify   *
 *  it under the terms of the GNU General Public License as published by   *
 *  the Free Software Foundation, version 2 of the License.                *
 *                                                                         *
 *  RobotStation is distributed in the hope that it will be useful         *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the           *
 *  GNU General Public License for more details.                           *
 *                                                                         *
 *  You should have received a copy of the GNU General Public License      *
 *  along with RobotStation. If not, see <http://www.gnu.org/licenses/>.   *
 *                                                                         *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "cameracontrolwidget.h"
#include "tools/snapshotdialog.h"
#include "core.h"

CameraControlWidget::CameraControlWidget(QWidget *parent) :
    QWidget(parent)
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);

    // Select camera
    // ********************************************************************************
    QSettings settings("RobotStation");
    int camera = settings.value("camera",0).toInt();

    QHBoxLayout *cameraSelectLayout = new QHBoxLayout(this);
    QLabel *cameraSelectLabel = new QLabel(tr("Camera"),this);
    cameraSelectLabel->setFixedWidth(100);

    m_cameraComboBox = new QComboBox(this);
    m_cameraComboBox->addItem("0",QVariant(0));
    m_cameraComboBox->addItem("1",QVariant(1));

    m_cameraComboBox->setCurrentIndex(camera);

    connect(m_cameraComboBox,SIGNAL(activated(int)),this,SLOT(updateCamera(int)));

    cameraSelectLayout->addWidget(cameraSelectLabel);
    cameraSelectLayout->addWidget(m_cameraComboBox);

    // Video Start Stop
    // ********************************************************************************
    QHBoxLayout *videoLayout = new QHBoxLayout(this);
    QPushButton *startVideo = new QPushButton("Start video", this);
    QPushButton *stopVideo = new QPushButton("Stop video", this);

    videoLayout->addWidget(startVideo);
    videoLayout->addWidget(stopVideo);

    connect(startVideo,SIGNAL(clicked()),this,SLOT(startVideo()));
    connect(stopVideo,SIGNAL(clicked()),this,SLOT(stopVideo()));

    // Flip display image
    // ********************************************************************************
    QCheckBox *flipHorizontalBox = new QCheckBox("Flip horizpntal",this);
    QCheckBox *flipVerticalBox = new   QCheckBox("Flip vertical",this);

    settings.beginGroup("flipimage");
    flipHorizontalBox->setChecked(settings.value("horizontal",false).toBool());
    flipVerticalBox->setChecked(settings.value("vertical",false).toBool());
    settings.endGroup();

    connect(flipHorizontalBox,SIGNAL(toggled(bool)),this,SLOT(updateHorizontalFlip(bool)));
    connect(flipVerticalBox,SIGNAL(toggled(bool)),this,SLOT(updateVerticalFlip(bool)));

    // Take Snapshot
    // ********************************************************************************
    QPushButton *takeSnapshotButton = new QPushButton("Take snapshot");
    connect(takeSnapshotButton,SIGNAL(clicked()),this,SLOT(startSnapshotDialog()));

    // process selection
    // ********************************************************************************
    int processType = settings.value("processType",0).toInt();

    QHBoxLayout *processLayout = new QHBoxLayout(this);
    QLabel *processLabel = new QLabel(tr("Process"),this);
    processLabel->setFixedWidth(100);

    m_imageComboBox = new QComboBox(this);
    m_imageComboBox->addItem("Original image",QVariant(0));
    m_imageComboBox->addItem("Result image",QVariant(1));

    m_imageComboBox->setCurrentIndex(processType);

    connect(m_imageComboBox,SIGNAL(activated(int)),this,SLOT(updateProcessType(int)));

    processLayout->addWidget(processLabel);
    processLayout->addWidget(m_imageComboBox);

    // Set Fps
    // ********************************************************************************
    QHBoxLayout *fpsLayout = new QHBoxLayout(this);
    QLabel *fpsDescriptionLabel = new QLabel(tr("fps"),this);
    fpsDescriptionLabel->setFixedWidth(100);
    m_fpsValueLabel = new QLabel("20",this);
    m_fpsValueLabel->setFixedWidth(40);

    m_fpsSlider = new QSlider(Qt::Horizontal,this);
    m_fpsSlider->setMinimum(1);
    m_fpsSlider->setMaximum(30);
    m_fpsSlider->setTickInterval(1);
    m_fpsSlider->setTickPosition(QSlider::TicksBothSides);
    m_fpsSlider->setValue(20);
    m_fpsSlider->setMinimumWidth(200);

    connect(m_fpsSlider,SIGNAL(valueChanged(int)),this,SLOT(updateFps(int)));

    fpsLayout->addWidget(fpsDescriptionLabel);
    fpsLayout->addWidget(m_fpsSlider);
    fpsLayout->addWidget(m_fpsValueLabel);

    // Contrast
    // ********************************************************************************
    QHBoxLayout *contrastLayout = new QHBoxLayout(this);
    double contrast = settings.value("contrast",1).toDouble();

    QLabel *contrastLabel = new QLabel(tr("Contrast: "),this);
    contrastLabel->setFixedWidth(100);

    m_contrastValueLabel = new QLabel(QString::number(contrast),this);
    m_contrastValueLabel->setFixedWidth(40);

    m_contrastSlider = new QSlider(Qt::Horizontal,this);
    m_contrastSlider->setMinimum(1);
    m_contrastSlider->setMaximum(300);
    m_contrastSlider->setTickInterval(10);
    m_contrastSlider->setTickPosition(QSlider::TicksBothSides);
    m_contrastSlider->setValue(contrast*100);

    connect(m_contrastSlider,SIGNAL(valueChanged(int)),this,SLOT(updateContrast(int)));

    contrastLayout->addWidget(contrastLabel);
    contrastLayout->addWidget(m_contrastSlider);
    contrastLayout->addWidget(m_contrastValueLabel);

    // Brightness
    // ********************************************************************************
    QHBoxLayout *brightnessLayout = new QHBoxLayout(this);
    double brightness = settings.value("brightness",0).toDouble();

    QLabel *brightnessLabel = new QLabel(tr("Brightness: "),this);
    brightnessLabel->setFixedWidth(100);

    m_brightnessValueLabel = new QLabel(QString::number(brightness),this);
    m_brightnessValueLabel->setFixedWidth(40);

    m_brightnessSlider = new QSlider(Qt::Horizontal,this);
    m_brightnessSlider->setMinimum(-255);
    m_brightnessSlider->setMaximum(255);
    m_brightnessSlider->setTickInterval(10);
    m_brightnessSlider->setTickPosition(QSlider::TicksBothSides);
    m_brightnessSlider->setValue(brightness);

    connect(m_brightnessSlider,SIGNAL(valueChanged(int)),this,SLOT(updateBrightness(int)));

    brightnessLayout->addWidget(brightnessLabel);
    brightnessLayout->addWidget(m_brightnessSlider);
    brightnessLayout->addWidget(m_brightnessValueLabel);

    // ********************************************************************************

    mainLayout->addLayout(cameraSelectLayout);
    mainLayout->addLayout(videoLayout);
    mainLayout->addWidget(flipHorizontalBox);
    mainLayout->addWidget(flipVerticalBox);
    mainLayout->addWidget(takeSnapshotButton);
    mainLayout->addLayout(processLayout);
    mainLayout->addLayout(fpsLayout);
    mainLayout->addLayout(brightnessLayout);
    mainLayout->addLayout(contrastLayout);

    setLayout(mainLayout);
}

void CameraControlWidget::startVideo()
{
    Core::instance()->cameraEngine()->startEngine();
}

void CameraControlWidget::stopVideo()
{
    Core::instance()->cameraEngine()->stopEngine();
}

void CameraControlWidget::startSnapshotDialog()
{
    SnapshotDialog snapshotDialog(0,Core::instance()->window()->image());
    snapshotDialog.exec();
}

void CameraControlWidget::updateCamera(const int &camera)
{
    Core::instance()->cameraEngine()->updateCamera(camera);
}

void CameraControlWidget::updateProcessType(const int &processType)
{
    Core::instance()->imageProcessor()->updateProcessType(processType);
}

void CameraControlWidget::updateFps(const int &fps)
{
    m_fpsValueLabel->setText(QString::number(fps));
    Core::instance()->imageProcessor()->updateFps(fps);
}

void CameraControlWidget::updateBrightness(const int &brightness)
{
    m_brightnessValueLabel->setText(QString::number(brightness));
    Core::instance()->imageProcessor()->updateBrightness(brightness);
}

void CameraControlWidget::updateContrast(const int &contrast)
{
    double contrastPercentage = ((double)contrast)/100;
    m_contrastValueLabel->setText(QString::number(contrastPercentage));
    Core::instance()->imageProcessor()->updateContrast(contrastPercentage);
}

void CameraControlWidget::updateHorizontalFlip(const bool &flip)
{
    Core::instance()->imageProcessor()->updateHorizontalFlip(flip);
}

void CameraControlWidget::updateVerticalFlip(const bool &flip)
{
    Core::instance()->imageProcessor()->updateVerticalFlip(flip);
}
