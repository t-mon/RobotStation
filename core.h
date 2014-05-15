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

#ifndef CORE_H
#define CORE_H

#include <QObject>

#include "gui/mainwindow.h"
#include "computerVision/cameraengine.h"
#include "computerVision/imageprocessor.h"
#include "robotCommunication/robotcommander.h"
#include "computerVision/poseestimationengine.h"

class Core : public QObject
{
    Q_OBJECT
public:
    static Core *instance() {
        if(!s_instance) {
            s_instance = new Core();
            s_instance->init();
        }
        return s_instance;
    }

    CameraEngine *cameraEngine();
    ImageProcessor *imageProcessor();
    MainWindow *window();
    RobotCommander *robot();
    PoseEstimationEngine *poseEngine();

private:
    explicit Core(QObject *parent = 0);
    static Core *s_instance;

    CameraEngine *m_cameraEngine;
    ImageProcessor *m_imageProcessor;
    MainWindow *m_window;
    RobotCommander *m_robot;
    PoseEstimationEngine *m_poseEngine;

private slots:
    void init();

signals:

public slots:

};

#endif // CORE_H
