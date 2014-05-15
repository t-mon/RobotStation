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

#include "snapshotdialog.h"
#include "core.h"

SnapshotDialog::SnapshotDialog(QWidget *parent, QImage snapshot) :
    QDialog(parent),m_image(snapshot)
{
    m_filePath = QDir::homePath();

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    setWindowTitle("Save snapshot...");

    m_imageLabel = new QLabel(this);
    m_imageLabel->setAutoFillBackground(true);
    m_imageLabel->setFixedSize(640,480);
    m_imageLabel->setPixmap(QPixmap::fromImage(m_image));

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    m_saveButton = new QPushButton("Save");
    m_cancelButton = new QPushButton("Cancel");
    buttonLayout->addWidget(m_saveButton);
    buttonLayout->addWidget(m_cancelButton);

    mainLayout->addWidget(m_imageLabel);
    mainLayout->addLayout(buttonLayout);

    connect(m_saveButton,SIGNAL(clicked()),this,SLOT(saveImage()));
    connect(m_cancelButton,SIGNAL(clicked()),this,SLOT(reject()));
}

void SnapshotDialog::saveImage()
{
    QString fileName = QFileDialog::getSaveFileName(this,tr("Save Snapshot"),m_filePath,tr("JPEG (*.jpeg)"));
    if(fileName.right(5) != ".jpeg"){
        fileName.append(".jpeg");
    }
    QPixmap image = QPixmap::fromImage(m_image);
    qDebug() << "save image to" << fileName;
    if(image.save(fileName)){
        Core::instance()->window()->writeToTerminal("Snapshot saved to " + fileName);
        reject();
    }else{
        Core::instance()->window()->writeErrorToTerminal("ERROR: could not save snapshot to " + fileName);
        reject();
    }
}
