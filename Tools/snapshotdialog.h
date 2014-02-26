#ifndef SNAPSHOTDIALOG_H
#define SNAPSHOTDIALOG_H
#include <QDialog>
#include <QLabel>
#include <QDebug>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFileDialog>

class SnapshotDialog : public QDialog
{
    Q_OBJECT
public:
    explicit SnapshotDialog(QWidget *parent = 0, QImage snapshot = QImage());

    QLabel *m_imageLabel;
    QImage m_image;

    QPushButton *m_saveButton;
    QPushButton *m_cancelButton;

    QString m_filePath;

signals:

private slots:
    void saveImage();

public slots:

};

#endif // SNAPSHOTDIALOG_H
