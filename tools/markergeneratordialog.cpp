#include "markergeneratordialog.h"
#include "core.h"

#include <QIcon>

MarkerGeneratorDialog::MarkerGeneratorDialog(QWidget *parent) :
    QDialog(parent)
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    setWindowTitle("Marker generator...");
    setWindowIcon(QIcon(":/markergenerator-icon.png"));

    // Image
    m_imageLabel = new QLabel(this);
    m_imageLabel->setAutoFillBackground(true);
    m_imageLabel->setFixedSize(497,497);
    
    // ID
    m_id = 0;
    QHBoxLayout *codeLayout = new QHBoxLayout();
    m_idLabel = new QLabel(tr("ID: 0"),this);
    m_idLabel->setFixedWidth(120);
    
    m_idSlider = new QSlider(Qt::Horizontal,this);
    m_idSlider->setMinimum(0);
    m_idSlider->setMaximum(4095);
    m_idSlider->setTickInterval(1);
    m_idSlider->setTickPosition(QSlider::TicksBothSides);
    m_idSlider->setValue(m_id);
    
    connect(m_idSlider,SIGNAL(valueChanged(int)),this,SLOT(idChanged(int)));
    
    codeLayout->addWidget(m_idLabel);
    codeLayout->addWidget(m_idSlider);
    
    
    // Size
    m_imageSize = 700;
    
    QHBoxLayout *sizeLayout = new QHBoxLayout();
    m_sizeLabel = new QLabel(tr("Size: 700x700"),this);
    m_sizeLabel->setFixedWidth(120);
    
    m_sizeSlider = new QSlider(Qt::Horizontal,this);
    m_sizeSlider->setMinimum(7);
    m_sizeSlider->setMaximum(1400);
    m_sizeSlider->setSingleStep(7);
    m_sizeSlider->setTickInterval(7);
    m_sizeSlider->setTickPosition(QSlider::TicksBothSides);
    m_sizeSlider->setValue(m_imageSize);
    
    connect(m_sizeSlider,SIGNAL(valueChanged(int)),this,SLOT(imageSizeChaged(int)));
    
    sizeLayout->addWidget(m_sizeLabel);
    sizeLayout->addWidget(m_sizeSlider);
    
    // Buttons
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    m_saveButton = new QPushButton("Save");
    m_cancelButton = new QPushButton("Cancel");
    buttonLayout->addWidget(m_saveButton);
    buttonLayout->addWidget(m_cancelButton);
    
    mainLayout->addWidget(m_imageLabel);
    mainLayout->addLayout(codeLayout);
    mainLayout->addLayout(sizeLayout);
    mainLayout->addLayout(buttonLayout);
    
    connect(m_saveButton,SIGNAL(clicked()),this,SLOT(saveImage()));
    connect(m_cancelButton,SIGNAL(clicked()),this,SLOT(reject()));
    
    updateImage();
}



QByteArray MarkerGeneratorDialog::getBinCode()
{
    QByteArray bin = QByteArray::number(m_id,2);
    QByteArray result("000000000000");
    result.replace(result.count()-bin.count(),bin.count(),bin);
    //qDebug() << "result = " << result;
    return result;
}

QGenericMatrix<1, 7, int> MarkerGeneratorDialog::encode(QByteArray code)
{
    int gMatrixValues[4*7] = {1,1,0,1,
                              1,0,1,1,
                              1,0,0,0,
                              0,1,1,1,
                              0,1,0,0,
                              0,0,1,0,
                              0,0,0,1};

    QGenericMatrix<4,7,int> gMatrix(gMatrixValues);
    //qDebug() << gMatrix;

    QGenericMatrix<1,4,int> codeVector;
    for(int i = 0; i < code.count(); i++){
        codeVector(i,0) = QString(code.at(i)).toInt();
    }
    //qDebug() << codeMatrix;

    QGenericMatrix<1,7,int> encoded = gMatrix * codeVector;

    for(int i = 0; i < 7; i++){
        encoded(i,0) = encoded(i,0) %2;
    }

    qDebug() << code << "encoded =" << encoded;
    return encoded;
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
        Core::instance()->window()->writeErrorToTerminal("ERROR: could not save marker to " + fileName);
    }
}

void MarkerGeneratorDialog::idChanged(const int &id)
{
    m_id = id;
    updateImage();
}


void MarkerGeneratorDialog::imageSizeChaged(const int &imageSize)
{
    m_imageSize = imageSize;
    updateImage();
}

void MarkerGeneratorDialog::updateImage()
{
    m_idLabel->setText("ID: " + QString::number(m_id));
    m_sizeLabel->setText("Size: " + QString::number(m_imageSize) + "x" + QString::number(m_imageSize));


    /*  Code word -> | Dx1 | Dx2 | Dx3 | Dx4 | Dy1 | Dy2 | Dy3 | Dy4 | Dz1 | Dz2 | Dz3 | Dz4 | = 2^12 = maximal 4096
     *
     *  So we have: - frame has to be black (0)
     *              - 4  cells for orientation
     *              - 9  cells for pairity and correction
     *              - 12 cells for data
     */
    
    m_codeMat = Mat::zeros(m_imageSize,m_imageSize,CV_8UC1);
    
    int swidth = (int)((float) m_imageSize / 7);
    
    QByteArray bin = getBinCode();

    QByteArray datax=bin.left(4);
    QByteArray datay=bin.mid(4,4);
    QByteArray dataz=bin.right(4);

    qDebug() << m_id << "bin code -> " << datax << datay << dataz;

    // [p1, p2, d1, p3, d2, d3, d4]^T

    QGenericMatrix<1, 7, int> xCode = encode(datax);
    QGenericMatrix<1, 7, int> yCode = encode(datay);
    QGenericMatrix<1, 7, int> zCode = encode(dataz);

    qDebug() << "===========================================";

    // now we can draw our marker...

    /*  |-----|-----|-----|-----|-----|-----|-----|
     *  |  0  |  0  |  0  |  0  |  0  |  0  |  0  |
     *  |     |     |     |     |     |     |     |
     *  |-----|-----|-----|-----|-----|-----|-----|
     *  |  0  |  1  | Px1 | Dx1 | Px2 |  0  |  0  |
     *  |     |     |     |     |     |     |     |
     *  |-----|-----|-----|-----|-----|-----|-----|
     *  |  0  | Px3 | Dx2 | Dx3 | Dx4 | Py1 |  0  |
     *  |     |     |     |     |     |     |     |
     *  |-----|-----|-----|-----|-----|-----|-----|
     *  |  0  | Dy1 | Py2 | Py3 | Dy2 | Dy3 |  0  |
     *  |     |     |     |     |     |     |     |
     *  |-----|-----|-----|-----|-----|-----|-----|
     *  |  0  | Dy4 | Pz1 | Dz1 | Pz2 | Pz3 |  0  |
     *  |     |     |     |     |     |     |     |
     *  |-----|-----|-----|-----|-----|-----|-----|
     *  |  0  |  0  | Dz2 | Dz3 | Dz4 |  0  |  0  |
     *  |     |     |     |     |     |     |     |
     *  |-----|-----|-----|-----|-----|-----|-----|
     *  |  0  |  0  |  0  |  0  |  0  |  0  |  0  |
     *  |     |     |     |     |     |     |     |
     *  |-----|-----|-----|-----|-----|-----|-----|
     */

    // rotation
    Mat roi = m_codeMat(Rect((1)* swidth,(1)* swidth,swidth,swidth));
    roi.setTo(Scalar(255));

    // [p1, p2, d1, p3, d2, d3, d4]^T

    /******************************
     * data x
     ******************************/

    // p1
    if(xCode(0,0) == 1){
       roi = m_codeMat(Rect((2)* swidth,(1)* swidth,swidth,swidth));
       roi.setTo(Scalar(255));
    }

    // p2
    if(xCode(1,0) == 1){
       roi = m_codeMat(Rect((4)* swidth,(1)* swidth,swidth,swidth));
       roi.setTo(Scalar(255));
    }

    // d1
    if(xCode(2,0) == 1){
       roi = m_codeMat(Rect((3)* swidth,(1)* swidth,swidth,swidth));
       roi.setTo(Scalar(255));
    }

    // p3
    if(xCode(3,0) == 1){
       roi = m_codeMat(Rect((1)* swidth,(2)* swidth,swidth,swidth));
       roi.setTo(Scalar(255));
    }

    // d2
    if(xCode(4,0) == 1){
       roi = m_codeMat(Rect((2)* swidth,(2)* swidth,swidth,swidth));
       roi.setTo(Scalar(255));
    }

    // d3
    if(xCode(5,0) == 1){
       roi = m_codeMat(Rect((3)* swidth,(2)* swidth,swidth,swidth));
       roi.setTo(Scalar(255));
    }

    // d4
    if(xCode(6,0) == 1){
       roi = m_codeMat(Rect((4)* swidth,(2)* swidth,swidth,swidth));
       roi.setTo(Scalar(255));
    }

    /******************************
     * data y
     ******************************/

    // p1
    if(yCode(0,0) == 1){
       roi = m_codeMat(Rect((5)* swidth,(2)* swidth,swidth,swidth));
       roi.setTo(Scalar(255));
    }

    // p2
    if(yCode(1,0) == 1){
       roi = m_codeMat(Rect((2)* swidth,(3)* swidth,swidth,swidth));
       roi.setTo(Scalar(255));
    }

    // d1
    if(yCode(2,0) == 1){
       roi = m_codeMat(Rect((1)* swidth,(3)* swidth,swidth,swidth));
       roi.setTo(Scalar(255));
    }

    // p3
    if(yCode(3,0) == 1){
       roi = m_codeMat(Rect((3)* swidth,(3)* swidth,swidth,swidth));
       roi.setTo(Scalar(255));
    }

    // d2
    if(yCode(4,0) == 1){
       roi = m_codeMat(Rect((4)* swidth,(3)* swidth,swidth,swidth));
       roi.setTo(Scalar(255));
    }

    // d3
    if(yCode(5,0) == 1){
       roi = m_codeMat(Rect((5)* swidth,(3)* swidth,swidth,swidth));
       roi.setTo(Scalar(255));
    }

    // d4
    if(yCode(6,0) == 1){
       roi = m_codeMat(Rect((1)* swidth,(4)* swidth,swidth,swidth));
       roi.setTo(Scalar(255));
    }

    /******************************
     * data z
     ******************************/

    // p1
    if(zCode(0,0) == 1){
       roi = m_codeMat(Rect((2)* swidth,(4)* swidth,swidth,swidth));
       roi.setTo(Scalar(255));
    }

    // p2
    if(zCode(1,0) == 1){
       roi = m_codeMat(Rect((4)* swidth,(4)* swidth,swidth,swidth));
       roi.setTo(Scalar(255));
    }

    // d1
    if(zCode(2,0) == 1){
       roi = m_codeMat(Rect((3)* swidth,(4)* swidth,swidth,swidth));
       roi.setTo(Scalar(255));
    }

    // p3
    if(zCode(3,0) == 1){
       roi = m_codeMat(Rect((5)* swidth,(4)* swidth,swidth,swidth));
       roi.setTo(Scalar(255));
    }

    // d2
    if(zCode(4,0) == 1){
       roi = m_codeMat(Rect((2)* swidth,(5)* swidth,swidth,swidth));
       roi.setTo(Scalar(255));
    }

    // d3
    if(zCode(5,0) == 1){
       roi = m_codeMat(Rect((3)* swidth,(5)* swidth,swidth,swidth));
       roi.setTo(Scalar(255));
    }

    // d4
    if(zCode(6,0) == 1){
       roi = m_codeMat(Rect((4)* swidth,(5)* swidth,swidth,swidth));
       roi.setTo(Scalar(255));
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
