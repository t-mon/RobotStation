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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGridLayout>
#include <QMenuBar>
#include <QStatusBar>
#include <QGroupBox>
#include <QLabel>
#include <QSlider>
#include <QPushButton>
#include <QCheckBox>
#include <QComboBox>
#include <QTextEdit>
#include <QTabWidget>
#include <QDateTime>


class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    QImage image();

private:
    QWidget *m_centralWidget;

    // Menu
    QMenu *m_fileMenu;
    QMenu *m_toolMenu;
    QMenu *m_settingsMenu;

    QAction *m_exitAction;
    QAction *m_calibrateAction;
    QAction *m_qrCodeSearchAction;
    QAction *m_markerCreatorAction;
    QAction *m_markerSearchAction;
    QAction *m_settingsAction;
    QAction *m_toolCalibrationAction;

    QStatusBar *m_statusBar;

    // image
    QLabel *m_imageLabel;
    QImage m_image;

    // terminal
    QTextEdit *m_terminal;

    // robot pose
    QLabel *m_dx;
    QLabel *m_dy;
    QLabel *m_dz;

    QLabel *m_wx;
    QLabel *m_wy;
    QLabel *m_wz;

    // groupboxes
    QLabel *createImageLabel();
    QGroupBox *createTerminalGroupBox();
    QTabWidget *createTabWidget();
    QGroupBox *createTranslationGroupBox();
    QGroupBox *createRotationGroupBox();

    void createMenus();

signals:

private slots:
    void startCalibrationDialog();
    void startCodeSearchDialog();
    void startSnapshotDialog();
    void startMarkerGeneratorDialog();
    void startMarkerSearchDialog();
    void startToolCalibrationWizard();
    void startSettingsDialog();

public slots:
    void setStatusBarText(const QString &message);
    void writeToTerminal(const QString &message);
    void writeErrorToTerminal(const QString &message);
    void updateImage(const QImage &image);

    void updateTranslationVector(const QVector3D &translation);
    void updateRotationVector(const QVector3D &rotation);

};

#endif // MAINWINDOW_H
