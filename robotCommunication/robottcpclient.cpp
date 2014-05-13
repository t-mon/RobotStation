#include "robottcpclient.h"
#include "core.h"

RobotTcpClient::RobotTcpClient(QObject *parent) :
    QObject(parent)
{
    m_tcpSocket = new QTcpSocket(this);

    qDebug() << "----------------------------";
    qDebug() << "network interfaces:";
    foreach(const QNetworkInterface &interface, QNetworkInterface::allInterfaces()){
        qDebug() << "   -------------------------";
        qDebug() << "   name:   " << interface.name();
        if(!interface.addressEntries().isEmpty()){
            qDebug() << "   ip:     " << interface.addressEntries().first().ip().toString();
        }
        qDebug() << "   mac:    " << interface.hardwareAddress();
    }
    qDebug() << "----------------------------";

    connect(m_tcpSocket,SIGNAL(connected()),this,SLOT(readData()));
    connect(m_tcpSocket,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(socketError(QAbstractSocket::SocketError)));
}

void RobotTcpClient::connected()
{
    Core::instance()->window()->writeToTerminal("connected to"  + m_tcpSocket->peerName() + "   " +  m_tcpSocket->peerAddress().toString() + "on port: " + m_tcpSocket->peerPort());
    connect(m_tcpSocket,SIGNAL(readyRead()),this,SLOT(readData()));
    connect(m_tcpSocket,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(socketError(QAbstractSocket::SocketError)));
}

void RobotTcpClient::disconnected()
{
    Core::instance()->window()->writeToTerminal("disconnected from " + m_tcpSocket->peerAddress().toString());
}

void RobotTcpClient::readData()
{
    QByteArray data = m_tcpSocket->readAll();
    emit dataReady(data);
}

void RobotTcpClient::socketError(QAbstractSocket::SocketError socketError)
{
    Core::instance()->window()->writeErrorToTerminal("ERROR: " + socketError + m_tcpSocket->errorString());
    qDebug() <<  socketError << m_tcpSocket->errorString();
}

void RobotTcpClient::disconnectRobot()
{
    m_tcpSocket->close();
}

void RobotTcpClient::connectRobot(const QHostAddress &address, const int &port)
{
    Core::instance()->window()->writeToTerminal("trying to connect to host -> " + address.toString() + "  on port: " + QString::number(port) + " .....");
    m_tcpSocket->connectToHost(address,port);
}

void RobotTcpClient::sendData(const QByteArray &data)
{
    m_tcpSocket->write(data);
}

