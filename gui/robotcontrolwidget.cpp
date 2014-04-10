#include "robotcontrolwidget.h"
#include "core.h"

RobotControlWidget::RobotControlWidget(QWidget *parent) :
    QWidget(parent)
{
    QVBoxLayout *mainLayout = new QVBoxLayout();

    mainLayout->addWidget(createConnectionGroupBox());
    mainLayout->addWidget(createMovementsGroupBox());

    setLayout(mainLayout);
}

QGroupBox *RobotControlWidget::createConnectionGroupBox()
{
    QGroupBox *connectionGroupBox = new QGroupBox("Connection");
    QGridLayout *mainLayout = new QGridLayout();

    // load settings
    QSettings settings("RobotStation");
    QString address = settings.value("address","127.0.0.1").toString();
    int port = settings.value("port","55555").toInt();

    m_addressLabel = new QLabel("Host: ",this);
    m_addressLineEdit= new QLineEdit(this);
    m_addressLineEdit->setText(address);

    m_portLabel = new QLabel("Port: ",this);
    m_portLineEdit = new QLineEdit(this);
    m_portLineEdit->setText(QString::number(port));

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    m_connectButton = new QPushButton("Connect", this);
    m_disconnectButton = new QPushButton("Disconnect",this);

    connect(m_connectButton,SIGNAL(clicked()),this,SLOT(connectButtonClicked()));
    connect(m_disconnectButton,SIGNAL(clicked()),this,SLOT(disconnectButtonClicked()));

    buttonLayout->addWidget(m_connectButton);
    buttonLayout->addWidget(m_disconnectButton);

    mainLayout->addWidget(m_addressLabel,0,0);
    mainLayout->addWidget(m_addressLineEdit,0,1);
    mainLayout->addWidget(m_portLabel,1,0);
    mainLayout->addWidget(m_portLineEdit,1,1);
    mainLayout->addLayout(buttonLayout,2,0,1,2);


    connectionGroupBox->setLayout(mainLayout);
    return connectionGroupBox;
}

QGroupBox *RobotControlWidget::createMovementsGroupBox()
{
    QGroupBox *movementsGroupBox = new QGroupBox("Movements");
    QGridLayout *mainLayout = new QGridLayout();

    QPushButton *moveHomeButton = new QPushButton("Home",this);
    connect(moveHomeButton,SIGNAL(clicked()),this,SLOT(moveHomeButtonClicked()));

    QPushButton *moveSearchPositionButton = new QPushButton("Search Position",this);
    connect(moveSearchPositionButton,SIGNAL(clicked()),this,SLOT(moveSearchPositionButtonClicked()));

    mainLayout->addWidget(moveHomeButton,0,0);
    mainLayout->addWidget(moveSearchPositionButton,0,1);

    movementsGroupBox->setLayout(mainLayout);
    return movementsGroupBox;
}

void RobotControlWidget::connectButtonClicked()
{
    QSettings settings("RobotStation");
    settings.setValue("address",m_addressLineEdit->text());
    settings.setValue("port",m_portLineEdit->text());

    Core::instance()->robot()->connectRobot(m_addressLineEdit->text(),m_portLineEdit->text().toInt());
}

void RobotControlWidget::disconnectButtonClicked()
{
    Core::instance()->robot()->disconnectRobot();
}

void RobotControlWidget::moveHomeButtonClicked()
{
    Core::instance()->window()->writeToTerminal("move HOME clicked");
    Core::instance()->robot()->moveHome();
}

void RobotControlWidget::moveSearchPositionButtonClicked()
{
    Core::instance()->window()->writeToTerminal("move SEARCH POSITION clicked");
    Core::instance()->robot()->moveSearchPosition();
}
