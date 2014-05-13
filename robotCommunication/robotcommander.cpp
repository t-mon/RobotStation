#include "robotcommander.h"
#include "core.h"

RobotCommander::RobotCommander(QObject *parent) :
    QObject(parent)
{
    m_server = new RobotTcpServer(this);

    connect(m_server,SIGNAL(dataReady(QByteArray)),this,SLOT(parseRobotData(QByteArray)));
}

void RobotCommander::parseRobotData(const QByteArray &data)
{
    QJsonParseError error;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(data, &error);

    if(error.error != QJsonParseError::NoError) {
        qWarning() << "ERROR: json failed to parse data" << data << ":" << error.errorString();
    }

    qDebug() << jsonDoc.toJson();
}

void RobotCommander::connectRobot(const QString &address, const int &port)
{
//    QHostAddress host;
//    if(host.setAddress(address)){
//        m_tcpClient->connectRobot(host,port);
//    }else{
//        Core::instance()->window()->writeErrorToTerminal("ERROR: could not parse host address");
//    }
}

void RobotCommander::disconnectRobot()
{
    //m_server->disconnectRobot();
}

void RobotCommander::moveHome()
{

}

void RobotCommander::moveSearchPosition()
{

}
