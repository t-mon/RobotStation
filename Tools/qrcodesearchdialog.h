#ifndef QRCODESEARCHDIALOG_H
#define QRCODESEARCHDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QDebug>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/calib3d/calib3d.hpp>

#include <zbar.h>

using namespace cv;
using namespace zbar;

class QRCodeSearchDialog : public QDialog
{
    Q_OBJECT
public:
    explicit QRCodeSearchDialog(QWidget *parent = 0);

private:
    QLabel *m_imageLabel;
    QLabel *m_statusText;

    QPushButton *m_okButton;
    QPushButton *m_cancelButton;

signals:

public slots:
    void updateImage(const Mat &image);


};

#endif // QRCODESEARCHDIALOG_H
