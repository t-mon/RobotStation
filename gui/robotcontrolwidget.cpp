#include "robotcontrolwidget.h"
#include "core.h"

RobotControlWidget::RobotControlWidget(QWidget *parent) :
    QWidget(parent)
{
    QVBoxLayout *mainLayout = new QVBoxLayout();

    mainLayout->addWidget(createMovementsGroupBox());
    mainLayout->addWidget(createProcessGroupBox());

    setLayout(mainLayout);
}

QGroupBox *RobotControlWidget::createMovementsGroupBox()
{
    QGroupBox *movementsGroupBox = new QGroupBox("Movements");
    QGridLayout *mainLayout = new QGridLayout();

    QPushButton *moveHomeButton = new QPushButton("Home",this);
    connect(moveHomeButton,SIGNAL(clicked()),this,SLOT(moveHomeClicked()));

    QPushButton *moveSearchPositionButton = new QPushButton("Search Position",this);
    connect(moveSearchPositionButton,SIGNAL(clicked()),this,SLOT(moveSearchPositionClicked()));

    QPushButton *stopMovementButton = new QPushButton("Stop",this);
    connect(stopMovementButton,SIGNAL(clicked()),this,SLOT(stopMovementClicked()));

    QPushButton *resetButton = new QPushButton("Reset",this);
    connect(resetButton,SIGNAL(clicked()),this,SLOT(resetClicked()));

    mainLayout->addWidget(moveHomeButton,0,0,1,2);
    mainLayout->addWidget(moveSearchPositionButton,1,0,1,2);
    mainLayout->addWidget(stopMovementButton,2,0,1,2);
    mainLayout->addWidget(resetButton,3,0,1,2);

    movementsGroupBox->setLayout(mainLayout);
    return movementsGroupBox;
}

QGroupBox *RobotControlWidget::createProcessGroupBox()
{
    QGroupBox *processGroupBox = new QGroupBox("Process");
    QGridLayout *mainLayout = new QGridLayout();

    QPushButton *startProcessButton = new QPushButton("Start",this);
    connect(startProcessButton,SIGNAL(clicked()),this,SLOT(startProcessClicked()));

    QPushButton *stopProcessButton = new QPushButton("Stop",this);
    connect(stopProcessButton,SIGNAL(clicked()),this,SLOT(stopProcessClicked()));

    mainLayout->addWidget(startProcessButton,0,0);
    mainLayout->addWidget(stopProcessButton,0,1);

    processGroupBox->setLayout(mainLayout);
    return processGroupBox;
}

void RobotControlWidget::moveHomeClicked()
{
    Core::instance()->robot()->moveHome();
}

void RobotControlWidget::moveSearchPositionClicked()
{
    Core::instance()->robot()->moveSearchPosition();
}

void RobotControlWidget::stopMovementClicked()
{
    Core::instance()->robot()->emergencyStop();
}

void RobotControlWidget::startProcessClicked()
{
    Core::instance()->robot()->startProcess();
}

void RobotControlWidget::stopProcessClicked()
{
    Core::instance()->robot()->stopProcess();
}

void RobotControlWidget::resetClicked()
{
    Core::instance()->robot()->reset();
}


