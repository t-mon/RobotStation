#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QPushButton>
#include <QGroupBox>
#include <QLayout>
#include <QLabel>
#include <QDebug>
#include <QSettings>
#include <QSpinBox>

class SettingsDialog : public QDialog
{
    Q_OBJECT
public:
    explicit SettingsDialog(QWidget *parent = 0);

    QPushButton *m_saveButton;
    QPushButton *m_cancelButton;
    QGroupBox *createTcpOffsetGroupBox();
    QGroupBox *createCalibrationGroupBox();

    QSpinBox *m_dx;
    QSpinBox *m_dy;
    QSpinBox *m_dz;

    QSpinBox *m_wx;
    QSpinBox *m_wy;
    QSpinBox *m_wz;

signals:

private slots:
    void saveButtonClicked();

public slots:

};

#endif // SETTINGSDIALOG_H
