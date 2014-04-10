#ifndef ROBOTTCPCLIENT_H
#define ROBOTTCPCLIENT_H

#include <QObject>
#include <QDebug>
#include <QTcpSocket>
#include <QNetworkInterface>

class RobotTcpClient : public QObject
{
    Q_OBJECT
public:
    explicit RobotTcpClient(QObject *parent = 0);

private:
    QTcpSocket *m_tcpSocket;

signals:
    void dataReady(const QByteArray &data);

private slots:
    void connected();
    void disconnected();
    void readData();
    void socketError(QAbstractSocket::SocketError socketError);

public slots:
    void disconnectRobot();
    void connectRobot(const QHostAddress &address, const int &port);
    void sendData(const QByteArray &data);


};

#endif // ROBOTTCPCLIENT_H
