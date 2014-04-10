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
