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
    QList <QTcpSocket*> m_clients;

signals:
    void dataReady(const QByteArray &data);

private slots:
    void readyRead();
    void socketError(QAbstractSocket::SocketError error);
    void newConnection();
    void clientDisconnected();

public slots:
    void sendData(const QByteArray &data);


};

#endif // ROBOTTCPSERVER_H
