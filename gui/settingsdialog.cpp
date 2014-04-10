#include "settingsdialog.h"

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent)
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    setWindowTitle("Settings");
    setWindowIcon(QIcon(":/settings-icon.png"));
    setMinimumSize(640,480);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    m_saveButton = new QPushButton("Save");
    m_cancelButton = new QPushButton("Cancel");
    buttonLayout->addWidget(m_saveButton);
    buttonLayout->addWidget(m_cancelButton);

    mainLayout->addLayout(buttonLayout);

    connect(m_cancelButton,SIGNAL(clicked()),this,SLOT(reject()));
    connect(m_saveButton,SIGNAL(clicked()),this,SLOT(accept()));
}
