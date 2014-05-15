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

#ifndef MARKERSEARCHDIALOG_H
#define MARKERSEARCHDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QDebug>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>

#include <computerVision/markersearchengine.h>

class MarkerSearchDialog : public QDialog
{
    Q_OBJECT
public:
    explicit MarkerSearchDialog(QWidget *parent = 0);
    QLabel *m_imageLabel;
    QImage m_image;

    QPushButton *m_cancelButton;
    MarkerSearchEngine *m_markerSearchEngine;

private slots:
    void updateImage(Mat image);

};

#endif // MARKERSEARCHDIALOG_H
