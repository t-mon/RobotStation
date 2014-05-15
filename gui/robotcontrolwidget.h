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

#ifndef ROBOTCONTROLWIDGET_H
#define ROBOTCONTROLWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QLayout>
#include <QGridLayout>
#include <QLineEdit>
#include <QDebug>
#include <QPushButton>
#include <QGroupBox>


class RobotControlWidget : public QWidget
{
    Q_OBJECT
public:
    explicit RobotControlWidget(QWidget *parent = 0);

private:
    QGroupBox *createMovementsGroupBox();
    QGroupBox *createProcessGroupBox();

signals:

private slots:
    void moveHomeClicked();
    void moveSearchPositionClicked();
    void stopMovementClicked();
    void startProcessClicked();
    void stopProcessClicked();
    void resetClicked();


public slots:

};

#endif // ROBOTCONTROLWIDGET_H
