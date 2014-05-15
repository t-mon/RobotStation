#include "settingsdialog.h"
#include "core.h"

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent)
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    setWindowTitle("Settings");
    setWindowIcon(QIcon(":/settings-icon.png"));

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    m_saveButton = new QPushButton("Save");
    m_cancelButton = new QPushButton("Cancel");
    buttonLayout->addWidget(m_saveButton);
    buttonLayout->addWidget(m_cancelButton);

    mainLayout->addWidget(createTcpOffsetGroupBox());
    mainLayout->addWidget(createCalibrationGroupBox());
    mainLayout->addLayout(buttonLayout);

    connect(m_cancelButton,SIGNAL(clicked()),this,SLOT(reject()));
    connect(m_saveButton,SIGNAL(clicked()),this,SLOT(saveButtonClicked()));
}

QGroupBox *SettingsDialog::createTcpOffsetGroupBox()
{
    QGroupBox *offsetGroupBox = new QGroupBox("Offset from Camera to TCP coordinate system");
    QGridLayout *mainLayout = new QGridLayout();

    // TRANSLATION
    QGroupBox *translationGroupBox = new QGroupBox("Translation");
    QGridLayout *translationLayout = new QGridLayout();

    // x-Offset
    QLabel *dxLabel = new QLabel("x [mm]",this);
    dxLabel->setFixedWidth(60);
    m_dx = new QSpinBox;
    m_dx->setRange(-500, +500);
    m_dx->setSpecialValueText("x [mm]");

    // y-Offset
    QLabel *dyLabel = new QLabel("y [mm]",this);
    dyLabel->setFixedWidth(60);
    m_dy = new QSpinBox;
    m_dy->setRange(-500, +500);
    m_dy->setSpecialValueText("y [mm]");

    // z-Offset
    QLabel *dzLabel = new QLabel("z [mm]",this);
    dzLabel->setFixedWidth(60);
    m_dz = new QSpinBox;
    m_dz->setRange(-500, +500);
    m_dz->setSpecialValueText("z [mm]");

    translationLayout->addWidget(dxLabel,0,0);
    translationLayout->addWidget(m_dx,0,1);

    translationLayout->addWidget(dyLabel,1,0);
    translationLayout->addWidget(m_dy,1,1);

    translationLayout->addWidget(dzLabel,2,0);
    translationLayout->addWidget(m_dz,2,1);

    translationGroupBox->setLayout(translationLayout);

    // ROTATION
    QGroupBox *rotationGroupBox = new QGroupBox("Rotation");
    QGridLayout *rotationLayout = new QGridLayout();

    // x-Offset
    QLabel *wxLabel = new QLabel("x [°]",this);
    wxLabel->setFixedWidth(60);
    m_wx = new QSpinBox;
    m_wx->setRange(-180, +180);
    m_wx->setSpecialValueText("x [°]");

    // y-Offset
    QLabel *wyLabel = new QLabel("y [°]",this);
    wyLabel->setFixedWidth(60);
    m_wy = new QSpinBox;
    m_wy->setRange(-180, +180);
    m_wy->setSpecialValueText("y [°]");

    // z-Offset
    QLabel *wzLabel = new QLabel("z [°]",this);
    wzLabel->setFixedWidth(60);
    m_wz = new QSpinBox;
    m_wz->setRange(-180, +180);
    m_wz->setSpecialValueText("z [°]");

    rotationLayout->addWidget(wxLabel,0,0);
    rotationLayout->addWidget(m_wx,0,1);

    rotationLayout->addWidget(wyLabel,1,0);
    rotationLayout->addWidget(m_wy,1,1);

    rotationLayout->addWidget(wzLabel,2,0);
    rotationLayout->addWidget(m_wz,2,1);

    rotationGroupBox->setLayout(rotationLayout);

    QSettings settings("RobotStation");
    settings.beginGroup("TCP Offset");
    m_dx->setValue(settings.value("dx",0).toInt());
    m_dy->setValue(settings.value("dy",0).toInt());
    m_dz->setValue(settings.value("dz",0).toInt());
    m_wx->setValue(settings.value("wx",0).toInt());
    m_wy->setValue(settings.value("wy",0).toInt());
    m_wz->setValue(settings.value("wz",0).toInt());
    settings.endGroup();

    mainLayout->addWidget(translationGroupBox,0,0);
    mainLayout->addWidget(rotationGroupBox,0,1);

    offsetGroupBox->setLayout(mainLayout);
    return offsetGroupBox;
}

QGroupBox *SettingsDialog::createCalibrationGroupBox()
{
    QGroupBox *calibrationGroupBox = new QGroupBox("Cameracalibration Parameter");
    QHBoxLayout *mainLayout = new QHBoxLayout();

    QGroupBox *intrinsicGroupBox = new QGroupBox("Intrinsic");
    QVBoxLayout *intrinsicLayout = new QVBoxLayout();
    QLabel *intrinsicLabel = new QLabel(this);

    QSettings settings("RobotStation");
    QString intrinsicString;

    settings.beginGroup("intrinsic");
    intrinsicString.append("fx = " + settings.value("fx",0).toString() + "\n");
    intrinsicString.append("fy = " + settings.value("fy",0).toString() + "\n");
    intrinsicString.append("cx = " + settings.value("cx",0).toString() + "\n");
    intrinsicString.append("cy = " + settings.value("cy",0).toString());
    settings.endGroup();

    intrinsicLabel->setText(intrinsicString);

    intrinsicLayout->addWidget(intrinsicLabel);
    intrinsicGroupBox->setLayout(intrinsicLayout);


    QGroupBox *extrinsicGroupBox = new QGroupBox("Extrinsic");
    QVBoxLayout *extrinsicLayout = new QVBoxLayout();
    QLabel *extrinsicLabel = new QLabel(this);

    QString extrinsicString;

    settings.beginGroup("extrinsic");
    extrinsicString.append("k1 = " + settings.value("k1",0).toString() + "\n");
    extrinsicString.append("k2 = " + settings.value("k2",0).toString() + "\n");
    extrinsicString.append("k3 = " + settings.value("k3",0).toString() + "\n");
    extrinsicString.append("p1 = " + settings.value("p1",0).toString() + "\n");
    extrinsicString.append("p2 = " + settings.value("p2",0).toString());
    settings.endGroup();

    extrinsicLabel->setText(extrinsicString);

    extrinsicLayout->addWidget(extrinsicLabel);
    extrinsicGroupBox->setLayout(extrinsicLayout);




    mainLayout->addWidget(intrinsicGroupBox);
    mainLayout->addWidget(extrinsicGroupBox);

    calibrationGroupBox->setLayout(mainLayout);
    return calibrationGroupBox;
}

void SettingsDialog::saveButtonClicked()
{
    QSettings settings("RobotStation");
    settings.beginGroup("TCP Offset");
    settings.setValue("dx", m_dx->text());
    settings.setValue("dy", m_dy->text());
    settings.setValue("dz", m_dz->text());
    settings.setValue("wx", m_wx->text());
    settings.setValue("wy", m_wy->text());
    settings.setValue("wz", m_wz->text());
    settings.endGroup();

    Core::instance()->poseEngine()->loadOffsetParameter();

    accept();
}

