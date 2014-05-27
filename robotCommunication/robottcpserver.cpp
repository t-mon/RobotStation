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

#include "robottcpserver.h"
#include "core.h"

RobotTcpServer::RobotTcpServer(QObject *parent) :
    QObject(parent)
{
    qDebug() << "----------------------------";
    qDebug() << "network interfaces:";
    foreach(const QNetworkInterface &interface, QNetworkInterface::allInterfaces()){
        qDebug() << "   -------------------------";
        qDebug() << "   name:" << interface.name();
        if(!interface.addressEntries().isEmpty()){
            qDebug() << "   ip:     " << interface.addressEntries().first().ip().toString();
        }
        qDebug() << "   mac: " << interface.hardwareAddress();
    }
    qDebug() << "----------------------------";
}

void RobotTcpServer::readyRead()
{
    QTcpSocket *robot = qobject_cast<QTcpSocket*>(sender());
    QByteArray data = robot->readAll();
    qDebug() << "-----> data comming from" << robot->peerAddress().toString();
    emit dataReady(data);
}

void RobotTcpServer::socketError(QAbstractSocket::SocketError error)
{
    QTcpSocket *robot = qobject_cast<QTcpSocket*>(sender());
    Core::instance()->window()->writeErrorToTerminal(robot->errorString());
    qDebug() <<  error << robot->errorString();
}

void RobotTcpServer::newConnection()
{
    if(m_robotList.count() >= 1){
        Core::instance()->window()->writeErrorToTerminal("ERROR: a client tryed to connect, but the robot is allready connected!");
        return;
    }
    QTcpSocket *robot = m_server->nextPendingConnection();
    Core::instance()->window()->writeToTerminal("Robot (" + robot->peerAddress().toString() + ") connected.");
    Core::instance()->window()->setStatusBarText("Robot (" + robot->peerAddress().toString() + ") connected.");
    connect(robot,SIGNAL(readyRead()),this,SLOT(readyRead()));
    connect(robot,SIGNAL(disconnected()),this,SLOT(robotDisconnected()));
    connect(robot,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(socketError(QAbstractSocket::SocketError)));
    m_robotList.append(robot);
    emit robotConnectionStateChanged(true);
}

void RobotTcpServer::robotDisconnected()
{
    QTcpSocket *robot = qobject_cast<QTcpSocket*>(sender());
    Core::instance()->window()->writeErrorToTerminal("Robot (" + robot->peerAddress().toString() + ") disconnected.");
    Core::instance()->window()->setStatusBarText("Server listening ...");
    m_robotList.clear();
    emit robotConnectionStateChanged(false);
}

void RobotTcpServer::startServer()
{
    m_server = new QTcpServer(this);
    m_server->setMaxPendingConnections(1);
    m_server->listen(QHostAddress::Any, 2020);

    Core::instance()->window()->writeToTerminal("Server for robot started on port 2020.");
    Core::instance()->window()->setStatusBarText("Server listening ...");
    connect(m_server,SIGNAL(newConnection()),this,SLOT(newConnection()));

}

void RobotTcpServer::sendData(const QByteArray &data)
{
    foreach(QTcpSocket *robot,m_robotList){
        robot->write(data);
    }
}
