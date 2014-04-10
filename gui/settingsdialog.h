#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QPushButton>
#include <QLayout>
#include <QSettings>

class SettingsDialog : public QDialog
{
    Q_OBJECT
public:
    explicit SettingsDialog(QWidget *parent = 0);

    QPushButton *m_saveButton;
    QPushButton *m_cancelButton;

signals:

public slots:

};

#endif // SETTINGSDIALOG_H
