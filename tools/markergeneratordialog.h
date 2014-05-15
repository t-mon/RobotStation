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

#ifndef MARKERGENERATORDIALOG_H
#define MARKERGENERATORDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QDebug>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QTextEdit>
#include <QSlider>
#include <QDir>
#include <QFileDialog>
#include <QPushButton>

#include <QGenericMatrix>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;

class MarkerGeneratorDialog : public QDialog
{
    Q_OBJECT
public:
    explicit MarkerGeneratorDialog(QWidget *parent = 0);

private:
    QLabel *m_imageLabel;

    QLabel *m_idLabel;
    QSlider *m_idSlider;
    int m_id;

    QLabel *m_sizeLabel;
    QSlider *m_sizeSlider;
    int m_imageSize;

    QPushButton *m_saveButton;
    QPushButton *m_cancelButton;

    Mat m_codeMat;
    QImage m_codeImage;

    QByteArray getBinCode();
    QGenericMatrix<1,7,int> encode(QByteArray code);

    
private slots:
    void saveImage();
    void idChanged(const int &id);
    void imageSizeChaged(const int &imageSize);
    void updateImage();

public slots:

};

#endif // MARKERGENERATORDIALOG_H
