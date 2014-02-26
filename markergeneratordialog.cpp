#include "markergeneratordialog.h"
#include "core.h"

MarkerGeneratorDialog::MarkerGeneratorDialog(QWidget *parent) :
    QDialog(parent)
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    setWindowTitle("Create ArUco Marker...");

    m_imageSize = 1400;

    // Image
    m_imageLabel = new QLabel(this);
    m_imageLabel->setAutoFillBackground(true);
    m_imageLabel->setFixedSize(500,500);

    // Code
    QHBoxLayout *codeLayout = new QHBoxLayout();
    m_idLabel = new QLabel(tr("0"),this);
    m_idLabel->setFixedWidth(50);

    m_idSlider = new QSlider(Qt::Horizontal,this);
    m_idSlider->setMinimum(0);
    m_idSlider->setMaximum(1024);
    m_idSlider->setTickInterval(1);
    m_idSlider->setTickPosition(QSlider::TicksBothSides);
    m_idSlider->setValue(0);

    connect(m_idSlider,SIGNAL(valueChanged(int)),this,SLOT(createCodeImage(int)));

    codeLayout->addWidget(m_idLabel);
    codeLayout->addWidget(m_idSlider);

    // Buttons
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    m_saveButton = new QPushButton("Save");
    m_cancelButton = new QPushButton("Cancel");
    buttonLayout->addWidget(m_saveButton);
    buttonLayout->addWidget(m_cancelButton);

    mainLayout->addWidget(m_imageLabel);
    mainLayout->addLayout(codeLayout);
    mainLayout->addLayout(buttonLayout);

    connect(m_saveButton,SIGNAL(clicked()),this,SLOT(saveImage()));
    connect(m_cancelButton,SIGNAL(clicked()),this,SLOT(reject()));

    createCodeImage(0);
}

void MarkerGeneratorDialog::saveImage()
{
    QString fileName = QFileDialog::getSaveFileName(this,tr("Save Marker"),QDir::homePath(),tr("PNG (*.png)"));
    if(fileName.right(4) != ".png"){
        fileName.append(".png");
    }

    qDebug() << "save marker to" << fileName;

    QPixmap image = QPixmap::fromImage(m_codeImage);

    if(image.save(fileName)){
        Core::instance()->window()->writeToTerminal("Marker saved to " + fileName);
    }else{
        Core::instance()->window()->writeToTerminal("ERROR: could not save marker to " + fileName);
    }
}

void MarkerGeneratorDialog::createCodeImage(const int &id)
{
    m_idLabel->setText(QString::number(id));
    m_codeMat = Mat::zeros(m_imageSize,m_imageSize,CV_8UC1);

    /* SOURCE: http://sourceforge.net/p/aruco/code/HEAD/tree/aruco_create_marker.cpp#l31
     *
     * Creates an ar marker with the id specified. hamming code is employed
     * There are a total of 5 rows of 5 cols each
     * Each row encodes a total of 2 bits, so there are 2^10 bits:(0-1023)
     * Hamming code is employed for error detection/correction
     * The least significative bytes are first (from left-up to to right-bottom)
     * Example: id = 110
     *   bin code: 00 01 10 11 10
     *   Marker (least significative bit is the leftmost)
     *
     *   Note: The first bit, is the inverse of the hamming parity. This avoids the 0 0 0 0 0 to be valid
     *
     *   1st row encodes 00: 1 0 0 0 0 : hex 0x10
     *   2nd row encodes 01: 1 0 1 1 1 : hex 0x17
     *   3nd row encodes 10: 0 1 0 0 1 : hex 0x09
     *   4th row encodes 11: 0 1 1 1 0 : hex 0x0e
     *   5th row encodes 10: 0 1 0 0 1 : hex 0x09
     */

    int ids[4]={0x10,0x17,0x09,0x0e};

    int swidth = (int)((float) m_imageSize / 7);

    for (int y=0; y<5; y++){
        int index = (id >> 2 * (4 - y)) & 0x0003;
        int val = ids[index];

        for (int x=0; x<5; x++){
            Mat roi = m_codeMat(Rect((x+1)* swidth,(y+1)* swidth,swidth,swidth));
            if ((val >> 4 - x) & 0x0001){
                roi.setTo(Scalar(255));
            }else{
                roi.setTo(Scalar(0));
            }
        }
    }

    m_codeImage = Core::instance()->imageProcessor()->convertMatToQimage(m_codeMat);

    // show marker
    int w = m_imageLabel->width();
    int h = m_imageLabel->height();

    QPixmap pixmap = QPixmap::fromImage(m_codeImage);
    if(!pixmap.isNull()){
        m_imageLabel->setPixmap(pixmap.scaled(w,h,Qt::KeepAspectRatio));
    }
}
