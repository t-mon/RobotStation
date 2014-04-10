#ifndef ROBOTCONTROLWIDGET_H
#define ROBOTCONTROLWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QDebug>
#include <QPushButton>
#include <QGroupBox>

class RobotControlWidget : public QWidget
{
    Q_OBJECT
public:
    explicit RobotControlWidget(QWidget *parent = 0);

    QLabel *m_addressLabel;
    QLabel *m_portLabel;
    QLineEdit *m_addressLineEdit;
    QLineEdit *m_portLineEdit;

    QPushButton *m_connectButton;
    QPushButton *m_disconnectButton;


    QGroupBox *createConnectionGroupBox();
    QGroupBox *createMovementsGroupBox();


signals:

private slots:
    void connectButtonClicked();
    void disconnectButtonClicked();
    void moveHomeButtonClicked();
    void moveSearchPositionButtonClicked();

public slots:

};

#endif // ROBOTCONTROLWIDGET_H
