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

    m_server = new QTcpServer(this);
    m_server->listen(QHostAddress::Any, 2020);
    qDebug() << "Server for Epson running on port 2020";

    connect(m_server,SIGNAL(newConnection()),this,SLOT(newConnection()));

}

void RobotTcpServer::readyRead()
{
    QTcpSocket *client = qobject_cast<QTcpSocket*>(sender());
    qDebug() << "-----> data comming from" << client->peerAddress().toString();
    QByteArray data = client->readAll();
    qDebug() << data;

}

void RobotTcpServer::socketError(QAbstractSocket::SocketError error)
{
    QTcpSocket *client = qobject_cast<QTcpSocket*>(sender());
    Core::instance()->window()->writeErrorToTerminal("ERROR: " + client->errorString());
    qDebug() <<  error << client->errorString();
}

void RobotTcpServer::newConnection()
{
    QTcpSocket *client = m_server->nextPendingConnection();
    Core::instance()->window()->writeToTerminal("Robot connected: " + client->peerAddress().toString());
    connect(client,SIGNAL(readyRead()),this,SLOT(readyRead()));
    connect(client,SIGNAL(disconnected()),this,SLOT(clientDisconnected()));
    connect(client,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(socketError(QAbstractSocket::SocketError)));
    m_clients.append(client);
}


void RobotTcpServer::clientDisconnected()
{
    QTcpSocket *client = qobject_cast<QTcpSocket*>(sender());
    Core::instance()->window()->writeErrorToTerminal("Robot disconnected: " + client->peerAddress().toString());
}

void RobotTcpServer::sendData(const QByteArray &data)
{
    foreach(QTcpSocket *client, m_clients){
        client->write(data);
    }
}
