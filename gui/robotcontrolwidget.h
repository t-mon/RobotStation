#ifndef ROBOTCONTROLWIDGET_H
#define ROBOTCONTROLWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QLayout>
#include <QGridLayout>
#include <QLineEdit>
#include <QDebug>
#include <QPushButton>
#include <QGroupBox>


class RobotControlWidget : public QWidget
{
    Q_OBJECT
public:
    explicit RobotControlWidget(QWidget *parent = 0);

private:
    QGroupBox *createMovementsGroupBox();
    QGroupBox *createProcessGroupBox();

signals:

private slots:
    void moveHomeClicked();
    void moveSearchPositionClicked();
    void stopMovementClicked();
    void startProcessClicked();
    void stopProcessClicked();
    void resetClicked();


public slots:

};

#endif // ROBOTCONTROLWIDGET_H
