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
