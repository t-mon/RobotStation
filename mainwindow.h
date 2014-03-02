#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGridLayout>
#include <QMenuBar>
#include <QStatusBar>
#include <QGroupBox>
#include <QLabel>
#include <QSlider>
#include <QPushButton>
#include <QCheckBox>
#include <QComboBox>
#include <QTextEdit>


class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    QWidget *m_centralWidget;

    // Menu
    QMenu *m_fileMenu;
    QMenu *m_toolMenu;

    QAction *m_exitAction;
    QAction *m_calibrateAction;
    QAction *m_qrCodeSearchAction;
    QAction *m_markerCreatorAction;
    QAction *m_markerSearchAction;

    QStatusBar *m_statusBar;

    // image
    QLabel *m_imageLabel;
    QComboBox *m_imageComboBox;
    QImage m_image;

    // fps
    QLabel *m_fpsLabel;
    QSlider *m_fpsSlider;

    // threshold
    QLabel *m_thresholdValueLabel;
    QSlider *m_thresholdSlider;


    // contrast alpha
    QLabel *m_contrastValueLabel;
    QSlider *m_contrastSlider;

    // brightness beta
    QLabel *m_brightnessValueLabel;
    QSlider *m_brightnessSlider;

    // terminal
    QTextEdit *m_terminal;


    // groupboxes
    QGroupBox *createImageGroupBox();
    QGroupBox *createToolGroupBox();
    QGroupBox *createTerminalGroupBox();

    void createMenus();

signals:
    void fpsChanged(const int &fps);
    void processTypeChanged(const int &processType);
    void thresholdValueChanged(const int &threshold);
    void contrastValueChanged(const double &contrast);
    void brightnessValueChanged(const double &brightness);

    void startVideo();
    void stopVideo();

private slots:
    void startCalibrationDialog();
    void startCodeSearchDialog();
    void startSnapshotDialog();
    void startMarkerGeneratorDialog();
    void startMarkerSearchDialog();

    void updateFpsValue(const int &fps);
    void updateThresholdValue(const int &threshold);
    void updateBrightnessValue(const int &brightness);
    void updateContrastValue(const int &contrast);


public slots:
    void writeToTerminal(const QString &message);
    void updateImage(const QImage &image);

};

#endif // MAINWINDOW_H
