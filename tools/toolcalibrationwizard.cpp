#include "toolcalibrationwizard.h"
#include "core.h"

ToolCalibrationWizard::ToolCalibrationWizard()
{

    setWindowTitle("Tool Calibration Wizard");
    if(Core::instance()->robot()->connected()){
        addPage(createIntroPage());
        addPage(createSearchPage());
        addPage(createPlaceCoordinateSystemPage());

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

    QLabel *textLabel = new QLabel("In this wizzard the real offset between"
                                   "the camera coordinate system and the TCP"
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


    QLabel *textLabel = new QLabel("Please move the robot to the teached"
                                   "search position. You can do that by pressing"
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
    page->setTitle("Place coordinatesystem marker");

    QLabel *textLabel = new QLabel("Transformation matrix from the camera view...",this);
    textLabel->setWordWrap(true);

    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(textLabel);
    layout->addWidget(m_cameraTransformationLabel);
    page->setLayout(layout);
    return page;
}

void ToolCalibrationWizard::initState()
{
    m_searchCoordinateSystem = false;
}

void ToolCalibrationWizard::searchState()
{
}

void ToolCalibrationWizard::nextStep(const int &id)
{
    switch (id) {
    case 0:
        qDebug() << " -> init state";
        initState();
    case 1:
        qDebug() << " -> move to search point state";
        searchState();
        break;
    case 2:
        qDebug() << " -> place koordinate system state";
        break;
    case 3:
        m_searchCoordinateSystem = true;
        qDebug() << " -> place search coordinate system state";
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

void ToolCalibrationWizard::coordinateSystemFound(const QMatrix4x4 &cameraTransformation)
{
    if(!m_searchCoordinateSystem){
        return;
    }

    m_cameraTransformation = cameraTransformation;
    m_searchCoordinateSystem = false;
}

void ToolCalibrationWizard::robotPointReceived(const QVector3D &translation, const QVector3D &rotation)
{
    m_toolTransformation = Core::instance()->poseEngine()->calculateTransformationFromPoint(translation,rotation);
}
