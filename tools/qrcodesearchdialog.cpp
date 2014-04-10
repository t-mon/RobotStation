#include "qrcodesearchdialog.h"
#include "core.h"

QRCodeSearchDialog::QRCodeSearchDialog(QWidget *parent) :
    QDialog(parent)
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    setWindowTitle("QR Code search engine...");
    setWindowIcon(QIcon(":/qrcode-icon.png"));

    m_imageLabel = new QLabel(this);
    m_imageLabel->setAutoFillBackground(true);
    m_imageLabel->setFixedSize(640,480);

    m_statusText = new QLabel(this);
    m_statusText->setText("Searching for codes...\n\n");

    QHBoxLayout *buttonLayout = new QHBoxLayout(this);
    m_okButton = new QPushButton("OK");
    m_cancelButton = new QPushButton("Cancel");
    buttonLayout->addWidget(m_okButton);
    buttonLayout->addWidget(m_cancelButton);

    mainLayout->addWidget(m_imageLabel);
    mainLayout->addWidget(m_statusText);
    mainLayout->addLayout(buttonLayout);

    connect(m_okButton, SIGNAL(clicked()), this, SLOT(accept()));
    connect(m_cancelButton, SIGNAL(clicked()), this, SLOT(reject()));

    connect(Core::instance()->imageProcessor(), SIGNAL(imageReady(Mat)), this, SLOT(updateImage(Mat)));
}

void QRCodeSearchDialog::updateImage(const Mat &image)
{
    ImageScanner scanner;
    scanner.set_config(ZBAR_NONE, ZBAR_CFG_ENABLE, 1);

    Mat img = image.clone();
    Mat grayMat;
    cvtColor(img,grayMat,CV_BGR2GRAY);

    int width = grayMat.cols;
    int height = grayMat.rows;

    // Obtain image data
    uchar *raw = (uchar*)grayMat.data;

    // Wrap image data4
    Image zbarImage(width, height, "Y800", raw, width * height);

    // scan image for codes
    int symbolCounter = scanner.scan(zbarImage);

    // go through all symbols in the image
    if(symbolCounter!=0){
        for(Image::SymbolIterator symbol=zbarImage.symbol_begin(); symbol!=zbarImage.symbol_end(); ++symbol){
            m_statusText->setText("Searching for codes...\n" + QString::fromStdString(symbol->get_type_name()) + "   ->    " + QString::fromStdString(symbol->get_data()) + "\n\n");

            if(symbol->get_location_size() == 4 && QString::fromStdString(symbol->get_type_name()) == "QR-Code"){
                // Draw corner points
                for(int i = 0; i < symbol->get_location_size(); i++){
                    circle(img,Point(symbol->get_location_x(i), symbol->get_location_y(i)),3,Scalar(255, 0, 0),2,8,0);
                }

                //Draw border of found symbols
                line(img, Point(symbol->get_location_x(0), symbol->get_location_y(0)), Point(symbol->get_location_x(1), symbol->get_location_y(1)), Scalar(0, 255, 0), 2, 8, 0);
                line(img, Point(symbol->get_location_x(1), symbol->get_location_y(1)), Point(symbol->get_location_x(2), symbol->get_location_y(2)), Scalar(0, 255, 0), 2, 8, 0);
                line(img, Point(symbol->get_location_x(2), symbol->get_location_y(2)), Point(symbol->get_location_x(3), symbol->get_location_y(3)), Scalar(0, 255, 0), 2, 8, 0);
                line(img, Point(symbol->get_location_x(3), symbol->get_location_y(3)), Point(symbol->get_location_x(0), symbol->get_location_y(0)), Scalar(0, 255, 0), 2, 8, 0);

                // Draw diagonales
                line(img, Point(symbol->get_location_x(0), symbol->get_location_y(0)), Point(symbol->get_location_x(2), symbol->get_location_y(2)), Scalar(0, 0, 255), 2, 8, 0);
                line(img, Point(symbol->get_location_x(1), symbol->get_location_y(1)), Point(symbol->get_location_x(3), symbol->get_location_y(3)), Scalar(0, 0, 255), 2, 8, 0);
            }
        }
    }
    zbarImage.set_data(NULL, 0);

    // show snapshot
    int w = m_imageLabel->width();
    int h = m_imageLabel->height();

    QPixmap pixmap = QPixmap::fromImage(Core::instance()->imageProcessor()->convertMatToQimage(img));
    if(!pixmap.isNull()){
        m_imageLabel->setPixmap(pixmap.scaled(w,h,Qt::KeepAspectRatio));
    }
}
