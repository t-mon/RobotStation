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

#include "robotcommander.h"
#include "core.h"

RobotCommander::RobotCommander(QObject *parent) :
    QObject(parent)
{
    m_server = new RobotTcpServer(this);
    connect(m_server,SIGNAL(dataReady(QByteArray)),this,SLOT(processRobotMessage(QByteArray)));
    connect(Core::instance()->poseEngine(),SIGNAL(coordinateSystemFound(QMatrix4x4)),this,SLOT(coordinateSystemFound(QMatrix4x4)));
    connect(m_server,SIGNAL(robotConnectionStateChanged(bool)),this,SLOT(robotConnectionStateChanged(bool)));
    m_state = StateNothing;
    m_robotConnectionState = false;
    m_server->startServer();
    m_collectingCoordinateSystems = false;
}

bool RobotCommander::connected()
{
    return m_robotConnectionState;
}

void RobotCommander::parsePointInformation(QByteArray data)
{
    qDebug() << "parsing point " << data << "...";
    qDebug() << "----------------------------------------";

    QString xString = data.mid(data.indexOf("X:") + 2, data.indexOf("Y:")-2 - data.indexOf("X:"));
    float x = xString.toFloat();
    qDebug() << "x = " << x;

    QString yString = data.mid(data.indexOf("Y:") + 2, data.indexOf("Z:")-2 - data.indexOf("Y:"));
    float y = yString.toFloat();
    qDebug() << "y = " << y;

    QString zString = data.mid(data.indexOf("Z:") + 2, data.indexOf("U:")-2 - data.indexOf("Z:"));
    float z = zString.toFloat();
    qDebug() << "z = " << z;

    QString uString = data.mid(data.indexOf("U:") + 2, data.indexOf("V:")-2 - data.indexOf("U:"));
    float u = uString.toFloat();
    qDebug() << "u = " << u;

    QString vString = data.mid(data.indexOf("V:") + 2, data.indexOf("W:")-2 - data.indexOf("V:"));
    float v = vString.toFloat();
    qDebug() << "v = " << v;

    QString wString = data.mid(data.indexOf("W:") + 2, data.indexOf("/R")-2 - data.indexOf("W:"));
    float w = wString.toFloat();
    qDebug() << "w = " << w;
    qDebug() << "----------------------------------------";

    QVector3D translation = QVector3D(x,y,z);
    QVector3D rotation = QVector3D(u,v,w);

    emit robotPointReceived(translation,rotation);
}

void RobotCommander::processRobotMessage(const QByteArray &data)
{
    qDebug() << "robot send: " << data;
    if(data.contains("OK")){
        Core::instance()->window()->writeToTerminal("       ...OK.");
    }

    if(data.contains("J6F0")){
        qDebug() << "received point information...";
        Core::instance()->window()->writeToTerminal("robot: received point information...");
        parsePointInformation(data);
        return;
    }

    switch (m_state) {
    case StateInit:
        qDebug() << "-> State = Init";
        Core::instance()->window()->writeToTerminal("robot: initializing...");
        m_server->sendData("Init");
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
    case StateNothing:
    case StateDone:
        Core::instance()->window()->writeToTerminal("robot: sleeping... ZZZZzzz...");
        break;
    case StateStop:
        qDebug() << "-> State = Stop";
        Core::instance()->window()->writeToTerminal("robot STOPED!");
        m_state = StateNothing;
        break;
    default:
        break;
    }
}

void RobotCommander::coordinateSystemFound(const QMatrix4x4 &transformationmatrix)
{
    if(m_collectingCoordinateSystems){
        m_coordinateSystem = transformationmatrix;
        qDebug() << "------------------------------";
        qDebug() << "Collected 1 coordinatesystems";

        m_collectingCoordinateSystems = false;
        qDebug() << "-> State = Send KKS";
        Core::instance()->window()->writeToTerminal("robot: sending coordinatesystem data ...");
        m_state = StateSendCoordinateSystem;
        sendCoordinateSystemData();
    }
}

void RobotCommander::robotConnectionStateChanged(bool connectionState)
{
    m_robotConnectionState = connectionState;
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
    m_state = StateSleeping;
}

void RobotCommander::moveSearchPosition()
{
    Core::instance()->window()->writeToTerminal("Move to Search Position");
    m_server->sendData("MoveSearch");
    m_state = StateSleeping;

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

    m_coordinateSystems.clear();
}

void RobotCommander::requestPointPosition()
{
    Core::instance()->window()->writeToTerminal("Request robot TCP position");
    m_server->sendData("GetTcpPoint");
}
