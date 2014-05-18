#include "toolcalibrationwizard.h"
#include "core.h"

ToolCalibrationWizard::ToolCalibrationWizard()
{

    setWindowTitle("Tool Calibration Wizard");
    if(Core::instance()->robot()->connected()){
        addPage(createIntroPage());
        addPage(createSearchPage());
        addPage(createPlaceCoordinateSystemPage());
        addPage(createFoundCoordinateSystemPage());
        addPage(createTeachCoordinateSystemPage());
        addPage(createTeachSystemFinishedPage());
        addPage(createGetTcpPositionPage());
        addPage(createCalculateOffsetPage());

        connect(this,SIGNAL(currentIdChanged(int)),this,SLOT(nextStep(int)));
        connect(Core::instance()->poseEngine(),SIGNAL(coordinateSystemFound(QMatrix4x4)),this,SLOT(coordinateSystemFound(QMatrix4x4)));
        connect(Core::instance()->robot(),SIGNAL(robotPointReceived(QVector3D,QVector3D)),this,SLOT(robotPointReceived(QVector3D,QVector3D)));
    }else{
        Core::instance()->window()->writeErrorToTerminal(" -> robot not connected.");
        addPage(createNotConnectedPage());
        reject();
    }

}

ToolCalibrationWizard::~ToolCalibrationWizard()
{
    this->deleteLater();
}

QWizardPage *ToolCalibrationWizard::createIntroPage()
{
    QWizardPage *page = new QWizardPage;
    page->setTitle("Tool Calibration Wizard");

    QLabel *textLabel = new QLabel("In this wizzard the real offset between "
                                   "the camera coordinate system and the TCP "
                                   "coordinate system of your tool.",this);
    textLabel->setWordWrap(true);

    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(textLabel);
    page->setLayout(layout);
    return page;
}

QWizardPage *ToolCalibrationWizard::createSearchPage()
{

    QWizardPage *page = new QWizardPage;
    page->setTitle("Move the search position");


    QLabel *textLabel = new QLabel("Please move the robot to the teached "
                                   "search position. You can do that by pressing "
                                   "following Button.",this);
    textLabel->setWordWrap(true);

    QPushButton *searchPositionButton = new QPushButton("Move to Search Position",this);
    connect(searchPositionButton,SIGNAL(clicked()),this,SLOT(searchPositionButtonClicked()));

    QLabel *text2Label = new QLabel("Click next if the robot is in the search position.",this);
    text2Label->setWordWrap(true);

    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(textLabel);
    layout->addWidget(searchPositionButton);
    layout->addWidget(text2Label);
    page->setLayout(layout);
    return page;
}

QWizardPage *ToolCalibrationWizard::createNotConnectedPage()
{
    QWizardPage *page = new QWizardPage;
    page->setTitle("ERROR: robot not connected");
    return page;
}

QWizardPage *ToolCalibrationWizard::createPlaceCoordinateSystemPage()
{
    QWizardPage *page = new QWizardPage;
    page->setTitle("Place coordinatesystem marker");

    QLabel *textLabel = new QLabel("Please the coordinate system somewhere in front "
                                   "of the robot where the camera can see it and the "
                                   "robot can reach it for teaching. If Robotstation "
                                   "found a coordinatesystem, click next...",this);
    textLabel->setWordWrap(true);

    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(textLabel);
    page->setLayout(layout);
    return page;
}

QWizardPage *ToolCalibrationWizard::createFoundCoordinateSystemPage()
{
    QWizardPage *page = new QWizardPage;
    page->setTitle("Searching coordinatesystem marker");

    QLabel *textLabel = new QLabel("Transformation matrix from the camera view...",this);
    textLabel->setWordWrap(true);

    m_cameraTransformationLabel = new QLabel("",this);

    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(textLabel);
    layout->addWidget(m_cameraTransformationLabel);
    page->setLayout(layout);
    return page;
}

QWizardPage *ToolCalibrationWizard::createTeachCoordinateSystemPage()
{
    QWizardPage *page = new QWizardPage;
    page->setTitle("Teach coordinatesystem with the robot to Local 1");

    QLabel *textLabel = new QLabel("Please teach the found coordinate system with the "
                                   "Robot to the Local 1 coordinate system. Follow the "
                                   "instruction in the Local Assistent of the Epson RC+5 Teach-"
                                   "tool.\n\n"
                                   "When you are finished with the teaching, click next..\n\n\n"
                                   "ATTENTION: do not move the coordinate system until the "
                                   "wizard is finished.",this);
    textLabel->setWordWrap(true);

    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(textLabel);
    page->setLayout(layout);
    return page;
}

QWizardPage *ToolCalibrationWizard::createTeachSystemFinishedPage()
{
    QWizardPage *page = new QWizardPage;
    page->setTitle("Move back the search position");


    QLabel *textLabel = new QLabel("Please move the robot back to the teached "
                                   "search position. You can do that by pressing "
                                   "following Button.",this);
    textLabel->setWordWrap(true);

    QPushButton *searchPositionButton = new QPushButton("Move to Search Position",this);
    connect(searchPositionButton,SIGNAL(clicked()),this,SLOT(searchPositionButtonClicked()));

    QLabel *text2Label = new QLabel("Click next if the robot is in the search position.",this);
    text2Label->setWordWrap(true);

    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(textLabel);
    layout->addWidget(searchPositionButton);
    layout->addWidget(text2Label);
    page->setLayout(layout);
    return page;
}

QWizardPage *ToolCalibrationWizard::createGetTcpPositionPage()
{
    QWizardPage *page = new QWizardPage;
    page->setTitle("Request the TCP pose in Locale 1");

    QLabel *textLabel = new QLabel("Press following button to request the TCP position "
                                   "in the new teached coordinatesystem",this);
    textLabel->setWordWrap(true);

    QPushButton *requestPositionButton = new QPushButton("Request TCP pose",this);
    connect(requestPositionButton,SIGNAL(clicked()),this,SLOT(requestPointButtonClicked()));

    QString text;
    text.append("x = \n");
    text.append("y = \n");
    text.append("z = \n");
    text.append("u = \n");
    text.append("v = \n");
    text.append("w = \n");

    m_toolPointLabel = new QLabel(text,this);

    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(textLabel);
    layout->addWidget(requestPositionButton);
    layout->addWidget(m_toolPointLabel);
    page->setLayout(layout);
    return page;
}

QWizardPage *ToolCalibrationWizard::createCalculateOffsetPage()
{
    QWizardPage *page = new QWizardPage;
    page->setTitle("Calculate TCP - Camera offset");

    QLabel *textLabel = new QLabel("The offset between the camera coordinate system "
                                   "and the TCP Koordinatesystem is:",this);
    textLabel->setWordWrap(true);

    QString text;
    text.append("x = \n");
    text.append("y = \n");
    text.append("z = \n");
    text.append("u = \n");
    text.append("v = \n");
    text.append("w = \n");

    m_offsetTransformationLabel = new QLabel(text,this);

    QLabel *text2Label = new QLabel("The parameter are saved and will be "
                                    "used from now on in the system. The parameter "
                                    "can be adjusted in the Settings dialog. ;)",this);
    textLabel->setWordWrap(true);

    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(textLabel);
    layout->addWidget(m_offsetTransformationLabel);
    layout->addWidget(text2Label);
    page->setLayout(layout);
    return page;
}

float ToolCalibrationWizard::roundValue(float value)
{
    return ((int)(value * 100 + .5) / 100.0);
}

void ToolCalibrationWizard::calculateOffset()
{
    m_offsetTransformation = m_cameraTransformation * m_toolTransformation;

    QVector3D translation = Core::instance()->poseEngine()->calculateTranslationFromTransformation(m_offsetTransformation);
    QVector3D rotation = Core::instance()->poseEngine()->calculateRotationFromTransformation(m_offsetTransformation);

    QString text;
    text.append("x = " + QString::number(translation.x()) + "\n");
    text.append("y = " + QString::number(translation.y()) + "\n");
    text.append("z = " + QString::number(translation.z()) + "\n");
    text.append("u = " + QString::number(rotation.x()) + "\n");
    text.append("v = " + QString::number(rotation.y()) + "\n");
    text.append("w = " + QString::number(rotation.z()) + "\n");

    m_offsetTransformationLabel->setText(text);
}

void ToolCalibrationWizard::saveOffset()
{
    QVector3D translation = Core::instance()->poseEngine()->calculateTranslationFromTransformation(m_offsetTransformation);
    QVector3D rotation = Core::instance()->poseEngine()->calculateRotationFromTransformation(m_offsetTransformation);

    QSettings settings("RobotStation");
    settings.beginGroup("TCP Offset");
    settings.setValue("dx", translation.x());
    settings.setValue("dy", translation.y());
    settings.setValue("dz", translation.z());
    settings.setValue("wx", rotation.x());
    settings.setValue("wy", rotation.y());
    settings.setValue("wz", rotation.z());
    settings.endGroup();

    Core::instance()->poseEngine()->loadOffsetParameter();
}

void ToolCalibrationWizard::nextStep(const int &id)
{
    switch (id) {
    case 0:
        qDebug() << " -> init state";
        m_searchCoordinateSystem = false;
        break;
    case 1:
        qDebug() << " -> move to search point state";
        break;
    case 2:
        qDebug() << " -> place koordinate system state";
        break;
    case 3:
        m_searchCoordinateSystem = true;
        qDebug() << " -> place search coordinate system state\n -> search coordinatesystem true";
        break;
    case 4:
        qDebug() << " -> teach coordinate system with robot, state";
        break;
    case 5:
        qDebug() << " -> move back to search position state";
        break;
    case 6:
        qDebug() << " -> request TCP point in new Local state";
        break;
    case 7:
        qDebug() << " -> calculate offset state";
        calculateOffset();
        saveOffset();
        break;
    case -1:
        qDebug() << " -> tool calibration wizard finished";
    default:
        break;
    }
}

void ToolCalibrationWizard::searchPositionButtonClicked()
{
    Core::instance()->robot()->moveSearchPosition();
}

void ToolCalibrationWizard::requestPointButtonClicked()
{
    Core::instance()->robot()->requestPointPosition();
}

void ToolCalibrationWizard::coordinateSystemFound(const QMatrix4x4 &cameraTransformation)
{
    if(!m_searchCoordinateSystem){
        return;
    }

    m_cameraTransformation = cameraTransformation;
    m_searchCoordinateSystem = false;

    QString text;
    text.append(QString::number(roundValue(m_cameraTransformation(0,0))));
    text.append("   ");
    text.append(QString::number(roundValue(m_cameraTransformation(0,1))));
    text.append("   ");
    text.append(QString::number(roundValue(m_cameraTransformation(0,2))));
    text.append("   ");
    text.append(QString::number(roundValue(m_cameraTransformation(0,3))) + "\n");

    text.append(QString::number(roundValue(m_cameraTransformation(1,0))));
    text.append("   ");
    text.append(QString::number(roundValue(m_cameraTransformation(1,1))));
    text.append("   ");
    text.append(QString::number(roundValue(m_cameraTransformation(1,2))));
    text.append("   ");
    text.append(QString::number(roundValue(m_cameraTransformation(1,3))) + "\n");

    text.append(QString::number(roundValue(m_cameraTransformation(2,0))));
    text.append("   ");
    text.append(QString::number(roundValue(m_cameraTransformation(2,1))));
    text.append("   ");
    text.append(QString::number(roundValue(m_cameraTransformation(2,2))));
    text.append("   ");
    text.append(QString::number(roundValue(m_cameraTransformation(2,3))) + "\n");

    text.append(QString::number(roundValue(m_cameraTransformation(3,0))));
    text.append("   ");
    text.append(QString::number(roundValue(m_cameraTransformation(3,1))));
    text.append("   ");
    text.append(QString::number(roundValue(m_cameraTransformation(3,2))));
    text.append("   ");
    text.append(QString::number(roundValue(m_cameraTransformation(3,3))) + "\n");


    m_cameraTransformationLabel->setText(text);
    qDebug() << "search coordinatesystem false";
}

void ToolCalibrationWizard::robotPointReceived(const QVector3D &translation, const QVector3D &rotation)
{
    QString text;
    text.append("x = " + QString::number(translation.x()) + "\n");
    text.append("y = " + QString::number(translation.y()) + "\n");
    text.append("z = " + QString::number(translation.z()) + "\n");
    text.append("u = " + QString::number(rotation.x()) + "\n");
    text.append("v = " + QString::number(rotation.y()) + "\n");
    text.append("w = " + QString::number(rotation.z()) + "\n");

    m_toolPointLabel->setText(text);

    m_toolTransformation = Core::instance()->poseEngine()->calculateTransformationFromPoint(translation,rotation);
}
