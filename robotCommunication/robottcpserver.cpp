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
    QByteArray data = m_robot->readAll();
    qDebug() << "-----> data comming from" << m_robot->peerAddress().toString()  << "\n" << data;
    emit dataReady(data);
}

void RobotTcpServer::socketError(QAbstractSocket::SocketError error)
{
    Core::instance()->window()->writeErrorToTerminal("ERROR: " + m_robot->errorString());
    qDebug() <<  error << m_robot->errorString();
}

void RobotTcpServer::newConnection()
{
    if(m_robot->state() == QAbstractSocket::ConnectedState){
        Core::instance()->window()->writeErrorToTerminal("ERROR: a client tryed to connect, but the robot is allready connected!");
        return;
    }
    m_robot = m_server->nextPendingConnection();
    Core::instance()->window()->writeToTerminal("Robot (" + m_robot->peerAddress().toString() + ") connected.");
    Core::instance()->window()->setStatusBarText("Robot (" + m_robot->peerAddress().toString() + ") connected.");
    connect(m_robot,SIGNAL(readyRead()),this,SLOT(readyRead()));
    connect(m_robot,SIGNAL(disconnected()),this,SLOT(robotDisconnected()));
    connect(m_robot,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(socketError(QAbstractSocket::SocketError)));
    sendData("Hello from RobotStation!");
}

void RobotTcpServer::robotDisconnected()
{
    Core::instance()->window()->writeErrorToTerminal("Robot (" + m_robot->peerAddress().toString() + ") disconnected.");
    Core::instance()->window()->setStatusBarText("Server listening ...");
    emit robotConnectionStatusChanged(false);
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
    m_robot->write(data);
}
