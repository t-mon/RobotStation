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

#ifndef CALIBRATIONDIALOG_H
#define CALIBRATIONDIALOG_H

#include <QDialog>
#include <QDebug>
#include <QPushButton>
#include <QGridLayout>
#include <QLabel>
#include <QSlider>
#include <QProgressBar>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/calib3d/calib3d.hpp>

using namespace cv;
using namespace std;

class CalibrationDialog : public QDialog
{
    Q_OBJECT
public:
    explicit CalibrationDialog(QWidget *parent = 0);
    QLabel *m_calibrationImageLabel;
    QLabel *m_statusText;

    QPushButton *m_saveButton;
    QPushButton *m_retryButton;
    QPushButton *m_cancelButton;

    QProgressBar *m_progressBar;
    int m_progress;

    // Frame steps for snapshots
    QSlider *m_fpsSlider;
    QLabel *m_frameStepDialog;

    // collecting cessboard images until we have x valid images
    int m_calibrationImages;

    // matrix for calibration parameters
    Mat m_intrinsic;
    Mat m_extrinsic;

    //2D image points
    vector<vector<Point2f> > m_imagePoints;

    //3D image points
    vector<vector<Point3f> > m_objectPoints;

    // size of the cessboard (9x6)
    Size m_patternSize;

    // number of corners in the cessboard -> 6*9 = 54
    int m_cornersNumber;

    // leght of a cessboard square in mm
    float m_sideLenght;

    void init();

private slots:
    void fpsChaged(const int &fps);
    void saveClicked();
    void retryClicked();
    void cancelClicked();

signals:

public slots:
    void updateImage(const Mat &image);

};

#endif // CALIBRATIONDIALOG_H
