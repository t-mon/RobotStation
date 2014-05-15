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

#ifndef ROBOTTCPSERVER_H
#define ROBOTTCPSERVER_H

#include <QObject>
#include <QDebug>
#include <QTcpSocket>
#include <QTcpServer>
#include <QNetworkInterface>

class RobotTcpServer : public QObject
{
    Q_OBJECT
public:
    explicit RobotTcpServer(QObject *parent = 0);

private:
    QTcpServer *m_server;
    QTcpSocket *m_robot;

signals:
    void dataReady(const QByteArray &data);
    void robotConnectionStatusChanged(const bool &status);

private slots:
    void readyRead();
    void socketError(QAbstractSocket::SocketError error);
    void newConnection();
    void robotDisconnected();

public slots:
    void startServer();
    void sendData(const QByteArray &data);


};

#endif // ROBOTTCPSERVER_H
