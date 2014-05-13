#ifndef ROBOTCOMMANDER_H
#define ROBOTCOMMANDER_H

#include <QObject>
#include <QJsonDocument>

#include "robotCommunication/robottcpserver.h"

class RobotCommander : public QObject
{
    Q_OBJECT
public:
    explicit RobotCommander(QObject *parent = 0);

private:
    RobotTcpServer *m_server;
    bool m_robotMoving;

signals:
    void robotMovingChanged(const bool &robotMoving);

private slots:
    void parseRobotData(const QByteArray &data);

public slots:
    void connectRobot(const QString &address, const int &port);
    void disconnectRobot();
    void moveHome();
    void moveSearchPosition();


};

#endif // ROBOTCOMMANDER_H
