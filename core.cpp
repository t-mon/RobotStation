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

#include "core.h"

Core *Core::s_instance = NULL;

Core::Core(QObject *parent) :
    QObject(parent)
{
}

void Core::init()
{
    m_window = new MainWindow(0);
    m_cameraEngine = new CameraEngine(0);
    m_imageProcessor = new ImageProcessor(m_window);
    m_poseEngine = new PoseEstimationEngine(m_window);
    m_robot = new RobotCommander(m_window);

    m_window->show();
    m_cameraEngine->startEngine();
}

CameraEngine *Core::cameraEngine()
{
    return m_cameraEngine;
}

ImageProcessor *Core::imageProcessor()
{
    return m_imageProcessor;
}

MainWindow *Core::window()
{
    return m_window;
}

RobotCommander *Core::robot()
{
    return m_robot;
}

PoseEstimationEngine *Core::poseEngine()
{
    return m_poseEngine;
}
