#ifndef MARKERSEARCHDIALOG_H
#define MARKERSEARCHDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QDebug>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>

#include <computerVision/markersearchengine.h>

class MarkerSearchDialog : public QDialog
{
    Q_OBJECT
public:
    explicit MarkerSearchDialog(QWidget *parent = 0);
    QLabel *m_imageLabel;
    QImage m_image;

    QPushButton *m_cancelButton;
    MarkerSearchEngine *m_markerSearchEngine;

private slots:
    void updateImage(Mat image);

};

#endif // MARKERSEARCHDIALOG_H
