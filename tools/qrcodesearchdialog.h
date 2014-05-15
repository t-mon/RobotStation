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

#ifndef QRCODESEARCHDIALOG_H
#define QRCODESEARCHDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QDebug>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/calib3d/calib3d.hpp>

#include <zbar.h>

using namespace cv;
using namespace zbar;

class QRCodeSearchDialog : public QDialog
{
    Q_OBJECT
public:
    explicit QRCodeSearchDialog(QWidget *parent = 0);

private:
    QLabel *m_imageLabel;
    QLabel *m_statusText;

    QPushButton *m_okButton;
    QPushButton *m_cancelButton;

signals:

public slots:
    void updateImage(const Mat &image);


};

#endif // QRCODESEARCHDIALOG_H
