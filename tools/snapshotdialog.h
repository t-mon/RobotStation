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

#ifndef SNAPSHOTDIALOG_H
#define SNAPSHOTDIALOG_H
#include <QDialog>
#include <QLabel>
#include <QDebug>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFileDialog>

class SnapshotDialog : public QDialog
{
    Q_OBJECT
public:
    explicit SnapshotDialog(QWidget *parent = 0, QImage snapshot = QImage());

    QLabel *m_imageLabel;
    QImage m_image;

    QPushButton *m_saveButton;
    QPushButton *m_cancelButton;

    QString m_filePath;

signals:

private slots:
    void saveImage();

public slots:

};

#endif // SNAPSHOTDIALOG_H
