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
