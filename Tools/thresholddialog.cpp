#include "thresholddialog.h"
#include "core.h"

ThresholdDialog::ThresholdDialog(QWidget *parent) :
    QDialog(parent)
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    setWindowTitle("Code search...");

    m_imageLabel = new QLabel(this);
    m_imageLabel->setAutoFillBackground(true);
    m_imageLabel->setFixedSize(640,480);

    connect(m_saveButton,SIGNAL(clicked()),this,SLOT(saveValues()));
    connect(m_cancelButton,SIGNAL(clicked()),this,SLOT(reject()));

    connect(Core::instance()->imageProcessor(), SIGNAL(imageReady(Mat)), this, SLOT(updateImage(Mat)));
}

void ThresholdDialog::updateImage(const Mat &image)
{



    // show image
    int w = m_imageLabel->width();
    int h = m_imageLabel->height();

    QPixmap pixmap = QPixmap::fromImage(Core::instance()->imageProcessor()->convertMatToQimage(image));
    if(!pixmap.isNull()){
        m_imageLabel->setPixmap(pixmap.scaled(w,h,Qt::KeepAspectRatio));
    }
}



void ThresholdDialog::saveValues()
{

}
