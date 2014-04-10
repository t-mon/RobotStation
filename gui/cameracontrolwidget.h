#ifndef CAMERACONTROLWIDGET_H
#define CAMERACONTROLWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QSlider>
#include <QComboBox>
#include <QLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QSettings>

class CameraControlWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CameraControlWidget(QWidget *parent = 0);

private:
    QComboBox *m_imageComboBox;
    // fps
    QLabel *m_fpsValueLabel;
    QSlider *m_fpsSlider;

    // contrast alpha
    QLabel *m_contrastValueLabel;
    QSlider *m_contrastSlider;

    // brightness beta
    QLabel *m_brightnessValueLabel;
    QSlider *m_brightnessSlider;


signals:

private slots:
    void startVideo();
    void stopVideo();

    void startSnapshotDialog();

    void updateProcessType(const int &processType);
    void updateFps(const int &fps);
    void updateBrightness(const int &brightness);
    void updateContrast(const int &contrast);


public slots:

};

#endif // CAMERACONTROLWIDGET_H
