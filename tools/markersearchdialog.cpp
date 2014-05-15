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

#include "markersearchdialog.h"
#include "core.h"

MarkerSearchDialog::MarkerSearchDialog(QWidget *parent) :
    QDialog(parent)
{
    m_markerSearchEngine = new MarkerSearchEngine(this);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    setWindowTitle("Find markers...");
    setModal(true);

    m_imageLabel = new QLabel(this);
    m_imageLabel->setAutoFillBackground(true);
    m_imageLabel->setFixedSize(640,480);
    m_imageLabel->setPixmap(QPixmap::fromImage(m_image));

    m_cancelButton = new QPushButton("Cancel");

    mainLayout->addWidget(m_imageLabel);
    mainLayout->addWidget(m_cancelButton);

    connect(m_cancelButton,SIGNAL(clicked()),this,SLOT(reject()));

    connect(Core::instance()->imageProcessor(), SIGNAL(imageReady(Mat)), this, SLOT(updateImage(Mat)));

}

void MarkerSearchDialog::updateImage(Mat image)
{
    Mat img = image.clone();
    QList<Marker> markerList = m_markerSearchEngine->searchMarker(img);
    m_markerSearchEngine->drawMarkers(img,markerList);

    // show snapshot
    int w = m_imageLabel->width();
    int h = m_imageLabel->height();

    QPixmap pixmap = QPixmap::fromImage(Core::instance()->imageProcessor()->convertMatToQimage(img));
    if(!pixmap.isNull()){
        m_imageLabel->setPixmap(pixmap.scaled(w,h,Qt::KeepAspectRatio));
    }
}
