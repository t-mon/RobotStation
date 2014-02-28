#ifndef THRESHOLDDIALOG_H
#define THRESHOLDDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QDebug>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;

class ThresholdDialog : public QDialog
{
    Q_OBJECT
public:
    explicit ThresholdDialog(QWidget *parent = 0);

private:
    QLabel *m_imageLabel;
    Mat m_image;

    QPushButton *m_saveButton;
    QPushButton *m_cancelButton;

signals:

private slots:
    void updateImage(const Mat &image);
    void saveValues();

public slots:

};

#endif // THRESHOLDDIALOG_H
