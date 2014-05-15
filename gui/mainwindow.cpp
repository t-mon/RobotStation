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

#include "mainwindow.h"

#include <QDebug>
#include <QSettings>

#include "tools/calibrationdialog.h"
#include "tools/markergeneratordialog.h"
#include "tools/snapshotdialog.h"
#include "tools/qrcodesearchdialog.h"
#include "tools/markersearchdialog.h"
#include "gui/settingsdialog.h"
#include "gui/robotcontrolwidget.h"
#include "gui/cameracontrolwidget.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{    
    setWindowTitle("Robot Station");
    setWindowIcon(QIcon(":/robotstation-icon.svg"));

    m_centralWidget = new QWidget(this);
    setCentralWidget(m_centralWidget);
    createMenus();

    m_statusBar = new QStatusBar(this);
    setStatusBar(m_statusBar);

    QGridLayout *mainLayout = new QGridLayout;
    m_centralWidget->setLayout(mainLayout);

    mainLayout->addWidget(createImageLabel(),0,0,2,1);
    mainLayout->addWidget(createTabWidget(),0,1,2,1);
    mainLayout->addWidget(createTranslationGroupBox(),0,2,1,1);
    mainLayout->addWidget(createRotationGroupBox(),1,2,1,1);
    mainLayout->addWidget(createTerminalGroupBox(),2,0,1,3);
}

MainWindow::~MainWindow()
{

}

QImage MainWindow::image()
{
    return m_image;
}

QLabel *MainWindow::createImageLabel()
{
    m_imageLabel = new QLabel(this);
    m_imageLabel->setAutoFillBackground(true);
    m_imageLabel->setFixedSize(580,435);
    return m_imageLabel;
}


QGroupBox *MainWindow::createTerminalGroupBox()
{
    QGroupBox *terminalGroupBox = new QGroupBox(tr("Terminal"),m_centralWidget);
    QVBoxLayout *terminalLayout = new QVBoxLayout();

    m_terminal = new QTextEdit(terminalGroupBox);
    m_terminal->setReadOnly(true);

    terminalLayout->addWidget(m_terminal);
    terminalGroupBox->setLayout(terminalLayout);
    terminalGroupBox->setMinimumHeight(200);

    return terminalGroupBox;
}

QTabWidget *MainWindow::createTabWidget()
{
    QTabWidget *tabs = new QTabWidget(this);

    RobotControlWidget *robotWidget = new RobotControlWidget(tabs);
    CameraControlWidget *cameraWidget = new CameraControlWidget(tabs);
    tabs->addTab(robotWidget,"Robot");
    tabs->addTab(cameraWidget,"Camera");

    return tabs;
}

QGroupBox *MainWindow::createTranslationGroupBox()
{
    QGroupBox *translationGroupBox = new QGroupBox(tr("Trans. [mm]"),m_centralWidget);
    QGridLayout *mainLayout = new QGridLayout;

    QLabel *dxLabel = new QLabel("x = ",this);
    m_dx = new QLabel("-",this);
    mainLayout->addWidget(dxLabel,0,0);
    mainLayout->addWidget(m_dx,0,1);

    QLabel *dyLabel = new QLabel("y = ",this);
    m_dy = new QLabel("-",this);
    mainLayout->addWidget(dyLabel,1,0);
    mainLayout->addWidget(m_dy,1,1);

    QLabel *dzLabel = new QLabel("z = ",this);
    m_dz = new QLabel("-",this);
    mainLayout->addWidget(dzLabel,2,0);
    mainLayout->addWidget(m_dz,2,1);

    translationGroupBox->setLayout(mainLayout);
    return translationGroupBox;
}

QGroupBox *MainWindow::createRotationGroupBox()
{
    QGroupBox *rotationGroupBox = new QGroupBox(tr("Rot. [°]"),m_centralWidget);
    QGridLayout *mainLayout = new QGridLayout;

    QLabel *wxLabel = new QLabel("x = ",this);
    m_wx = new QLabel("-",this);
    mainLayout->addWidget(wxLabel,0,0);
    mainLayout->addWidget(m_wx,0,1);

    QLabel *wyLabel = new QLabel("y = ",this);
    m_wy = new QLabel("-",this);
    mainLayout->addWidget(wyLabel,1,0);
    mainLayout->addWidget(m_wy,1,1);

    QLabel *wzLabel = new QLabel("z = ",this);
    m_wz = new QLabel("-",this);
    mainLayout->addWidget(wzLabel,2,0);
    mainLayout->addWidget(m_wz,2,1);

    rotationGroupBox->setLayout(mainLayout);
    return rotationGroupBox;
}

void MainWindow::createMenus()
{
    m_fileMenu = menuBar()->addMenu(tr("&File"));

    m_exitAction = new QAction(tr("E&xit"),this);
    m_fileMenu->addAction(m_exitAction);
    connect(m_exitAction,SIGNAL(triggered()),this,SLOT(close()));

    m_toolMenu = menuBar()->addMenu("&Tools");

    m_calibrateAction = new QAction(tr("&Calibrate camera"),this);
    m_toolMenu->addAction(m_calibrateAction);
    connect(m_calibrateAction,SIGNAL(triggered()),this,SLOT(startCalibrationDialog()));

    m_markerCreatorAction = new QAction(tr("&Marker generator"),this);
    m_toolMenu->addAction(m_markerCreatorAction);
    connect(m_markerCreatorAction,SIGNAL(triggered()),this,SLOT(startMarkerGeneratorDialog()));

    m_markerSearchAction = new QAction(tr("Marker &search engine"),this);
    m_toolMenu->addAction(m_markerSearchAction);
    connect(m_markerSearchAction,SIGNAL(triggered()),this,SLOT(startMarkerSearchDialog()));

    m_toolMenu->addSeparator();

    m_qrCodeSearchAction = new QAction(tr("&QR-code search engine"),this);
    m_toolMenu->addAction(m_qrCodeSearchAction);
    connect(m_qrCodeSearchAction,SIGNAL(triggered()),this,SLOT(startCodeSearchDialog()));

    m_settingsMenu = menuBar()->addMenu(tr("Settings"));

    m_settingsAction = new QAction(tr("Settings"),this);
    m_settingsMenu->addAction(m_settingsAction);
    connect(m_settingsAction,SIGNAL(triggered()),this,SLOT(startSettingsDialog()));
}

void MainWindow::startCalibrationDialog()
{
    CalibrationDialog calibrationDialog;
    calibrationDialog.exec();
}

void MainWindow::startCodeSearchDialog()
{
    QRCodeSearchDialog qrCodeSearchDialog;
    qrCodeSearchDialog.exec();
}

void MainWindow::startSnapshotDialog()
{
    SnapshotDialog snapshotDialog(0,m_image);
    snapshotDialog.exec();
}

void MainWindow::startMarkerGeneratorDialog()
{
    MarkerGeneratorDialog markerGenerator;
    markerGenerator.exec();
}

void MainWindow::startMarkerSearchDialog()
{
    MarkerSearchDialog markerSearch;
    markerSearch.exec();
}

void MainWindow::startSettingsDialog()
{
    SettingsDialog settingsDialog;
    settingsDialog.exec();
}

void MainWindow::setStatusBarText(const QString &message)
{
    m_statusBar->showMessage(message);
}

void MainWindow::writeToTerminal(const QString &message)
{
    qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss") << " -->  " <<  message;
    m_terminal->setTextColor(QColor(0, 255, 0,255));
    m_terminal->append(QDateTime::currentDateTime().toString("hh:mm:ss") + " -->  " +  message);
}

void MainWindow::writeErrorToTerminal(const QString &message)
{
    qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss") << " -->  " <<  message;
    m_terminal->setTextColor(QColor(255, 0, 0,255));
    m_terminal->append(QDateTime::currentDateTime().toString("hh:mm:ss") + " -->  " +  message);
}

void MainWindow::updateImage(const QImage &image)
{
    m_image = image;

    int w = m_imageLabel->width();
    int h = m_imageLabel->height();

    QPixmap pixmap = QPixmap::fromImage(image);
    if(!pixmap.isNull()){
        m_imageLabel->setPixmap(pixmap.scaled(w,h,Qt::KeepAspectRatio));
    }
}

void MainWindow::updateTranslationVector(const QVector3D &translation)
{
    m_dx->setText(QString::number(translation.x()));
    m_dy->setText(QString::number(translation.y()));
    m_dz->setText(QString::number(translation.z()));

}

void MainWindow::updateRotationVector(const QVector3D &rotation)
{
    m_wx->setText(QString::number(rotation.x()));
    m_wy->setText(QString::number(rotation.y()));
    m_wz->setText(QString::number(rotation.z()));
}
