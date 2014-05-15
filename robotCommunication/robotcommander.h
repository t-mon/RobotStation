#ifndef ROBOTCOMMANDER_H
#define ROBOTCOMMANDER_H

#include <QObject>
#include <QMatrix4x4>
#include "robotCommunication/robottcpserver.h"

class RobotCommander : public QObject
{
    Q_OBJECT
public:

    enum ProcessState{
        StateSleeping,
        StateInit,
        StateStart,
        StateStop,
        StateMoveHome,
        StateMoveSearch,
        StateCalculatePosition,
        StateCollectingPositions,
        StateSendCoordinateSystem,
        StateDrawing,
        StateFinishing,
        StateError,
        StateDone
    };

    explicit RobotCommander(QObject *parent = 0);

private:
    RobotTcpServer *m_server;
    ProcessState m_state;

    bool m_collectingCoordinateSystems;
    QList<QMatrix4x4> m_coordinateSystems;

signals:

private slots:
    void processRobotMessage(const QByteArray &data);
    void coordinateSystemFound(const QMatrix4x4 &transformationmatrix);

public slots:
    void startProcess();
    void stopProcess();

    void moveHome();
    void moveSearchPosition();
    void emergencyStop();
    void reset();

    void sendCoordinateSystemData();

};

#endif // ROBOTCOMMANDER_H
