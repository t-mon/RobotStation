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
