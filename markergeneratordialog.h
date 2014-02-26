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

    QPushButton *m_saveButton;
    QPushButton *m_cancelButton;

    Mat m_codeMat;
    QImage m_codeImage;

    int m_imageSize;

private slots:
    void saveImage();
    void createCodeImage(const int &id);

public slots:

};

#endif // MARKERGENERATORDIALOG_H
