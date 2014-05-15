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

#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QPushButton>
#include <QGroupBox>
#include <QLayout>
#include <QLabel>
#include <QDebug>
#include <QSettings>
#include <QSpinBox>

class SettingsDialog : public QDialog
{
    Q_OBJECT
public:
    explicit SettingsDialog(QWidget *parent = 0);

    QPushButton *m_saveButton;
    QPushButton *m_cancelButton;
    QGroupBox *createTcpOffsetGroupBox();
    QGroupBox *createCalibrationGroupBox();

    QSpinBox *m_dx;
    QSpinBox *m_dy;
    QSpinBox *m_dz;

    QSpinBox *m_wx;
    QSpinBox *m_wy;
    QSpinBox *m_wz;

signals:

private slots:
    void saveButtonClicked();

public slots:

};

#endif // SETTINGSDIALOG_H
