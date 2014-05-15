#include "robotcommander.h"
#include "core.h"

RobotCommander::RobotCommander(QObject *parent) :
    QObject(parent)
{
    m_server = new RobotTcpServer(this);
    connect(m_server,SIGNAL(dataReady(QByteArray)),this,SLOT(processRobotMessage(QByteArray)));
    connect(Core::instance()->poseEngine(),SIGNAL(coordinateSystemFound(QMatrix4x4)),this,SLOT(coordinateSystemFound(QMatrix4x4)));

    m_state = StateDone;
    m_server->startServer();
}

void RobotCommander::processRobotMessage(const QByteArray &data)
{
    qDebug() << "robot send: " << data;
//    if(data != "OK\n"){
//        qDebug() << "ERROR: something went wrong on robot";
//        return;
//    }

    switch (m_state) {
    case StateInit:
        qDebug() << "-> State = Init";
        Core::instance()->window()->writeToTerminal("robot: initializing...");
        m_server->sendData("Reset");
        m_state = StateStart;
        break;
    case StateStart:
        qDebug() << "-> State = Start";
        Core::instance()->window()->writeToTerminal("robot: moving to HOME position...");
        m_server->sendData("MoveHome");
        m_state = StateMoveSearch;
        break;
    case StateMoveSearch:
        qDebug() << "-> State = MoveSearch";
        Core::instance()->window()->writeToTerminal("robot: moving to SEARCH position...");
        m_server->sendData("MoveSearch");
        m_state = StateCalculatePosition;
        break;
    case StateCalculatePosition:
        // start collecting the coorinatesystems, when done, call sendCoordinateSystemData()
        Core::instance()->window()->writeToTerminal("robot: estimating mobile robot position...");
        qDebug() << "-> State = Calculate Position";
        m_coordinateSystems.clear();
        m_collectingCoordinateSystems = true;
        m_state = StateCollectingPositions;
        break;
    case StateDrawing:
        qDebug() << "-> State = Drawing";
        m_server->sendData("Draw");
        Core::instance()->window()->writeToTerminal("robot: drawing...");
        m_state = StateFinishing;
        break;
    case StateFinishing:
        qDebug() << "-> State = Finishing";
        Core::instance()->window()->writeToTerminal("robot: moving to HOME position...");
        m_server->sendData("MoveHome");
        m_state = StateSleeping;
        break;
    case StateSleeping:
        m_state = StateDone;
        break;
    case StateDone:
        Core::instance()->window()->writeToTerminal("robot: sleeping... ZZZZzzz...");
        break;
    case StateStop:
        qDebug() << "-> State = Stop";
        Core::instance()->window()->writeToTerminal("robot STOPED!");
        m_state = StateSleeping;
        break;
    default:
        break;
    }
}

void RobotCommander::coordinateSystemFound(const QMatrix4x4 &transformationmatrix)
{
    qDebug() << "Calcpos";
    if(m_collectingCoordinateSystems){
        m_coordinateSystems.append(transformationmatrix);
        if(m_coordinateSystems.count() >= 10){
            // if we have found 10 coordinatesystems
            qDebug() << "------------------------------";
            qDebug() << "Collected 10 coordinatesystems";

            m_collectingCoordinateSystems = false;
            qDebug() << "-> State = Send KKS";
            Core::instance()->window()->writeToTerminal("robot: sending coordinatesystem data ...");
            m_state = StateSendCoordinateSystem;
            sendCoordinateSystemData();
        }
    }
}

void RobotCommander::startProcess()
{
    Core::instance()->window()->writeToTerminal("Robot process started");
    m_server->sendData("Start");
    m_state = StateInit;

}

void RobotCommander::stopProcess()
{
    Core::instance()->window()->writeToTerminal("Robot process stopped");
    emergencyStop();
}


void RobotCommander::moveHome()
{
    Core::instance()->window()->writeToTerminal("Move to Home Position");
    m_server->sendData("MoveHome");
}

void RobotCommander::moveSearchPosition()
{
    Core::instance()->window()->writeToTerminal("Move to Search Position");
    m_server->sendData("MoveSearch");
}

void RobotCommander::emergencyStop()
{
    Core::instance()->window()->writeToTerminal("Stop robot movement");
    m_server->sendData("STOP");
    m_state = StateStop;
}

void RobotCommander::reset()
{
    Core::instance()->window()->writeToTerminal("Reset robot");
    m_server->sendData("Reset");
}

void RobotCommander::sendCoordinateSystemData()
{
    m_server->sendData("KKS data...bla bla");
    m_state = StateDrawing;
}
