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
    QAction *m_codeSearchAction;
    QAction *m_markerCreatorAction;

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

    // terminal
    QTextEdit *m_terminal;


    // groupboxes
    QGroupBox *createImageGroupBox();
    QGroupBox *createToolGroupBox();
    QGroupBox *createTerminalGroupBox();

    void createMenus();

signals:
    void fpsChanged(const int &fps);
    void thresholdValueChanged(const int &threshold);
    void processTypeChanged(const int &processType);
    void startVideo();
    void pauseVideo();

private slots:
    void startCalibrationDialog();
    void startCodeSearchDialog();
    void startSnapshotDialog();
    void startMarkerGeneratorDialog();

    void updateThresholdValue(const int &threshold);
    void updateFpsValue(const int &fps);

public slots:
    void writeToTerminal(const QString &message);
    void updateImage(const QImage &image);

};

#endif // MAINWINDOW_H
