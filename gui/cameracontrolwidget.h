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

#ifndef CAMERACONTROLWIDGET_H
#define CAMERACONTROLWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QSlider>
#include <QComboBox>
#include <QLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QSettings>

class CameraControlWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CameraControlWidget(QWidget *parent = 0);

private:
    QComboBox *m_cameraComboBox;
    QComboBox *m_imageComboBox;
    // fps
    QLabel *m_fpsValueLabel;
    QSlider *m_fpsSlider;

    // contrast alpha
    QLabel *m_contrastValueLabel;
    QSlider *m_contrastSlider;

    // brightness beta
    QLabel *m_brightnessValueLabel;
    QSlider *m_brightnessSlider;


signals:

private slots:
    void startVideo();
    void stopVideo();

    void startSnapshotDialog();

    void updateCamera(const int &camera);
    void updateProcessType(const int &processType);
    void updateFps(const int &fps);
    void updateBrightness(const int &brightness);
    void updateContrast(const int &contrast);


public slots:

};

#endif // CAMERACONTROLWIDGET_H
