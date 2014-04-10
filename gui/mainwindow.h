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
#include <QTabWidget>
#include <QDateTime>


class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    QImage image();

private:
    QWidget *m_centralWidget;

    // Menu
    QMenu *m_fileMenu;
    QMenu *m_toolMenu;
    QMenu *m_settingsMenu;

    QAction *m_exitAction;
    QAction *m_calibrateAction;
    QAction *m_qrCodeSearchAction;
    QAction *m_markerCreatorAction;
    QAction *m_markerSearchAction;
    QAction *m_settingsAction;

    QStatusBar *m_statusBar;

    // image
    QLabel *m_imageLabel;
    QImage m_image;

    // terminal
    QTextEdit *m_terminal;

    // groupboxes
    QLabel *createImageLabel();
    QGroupBox *createTerminalGroupBox();
    QTabWidget *createTabWidget();

    void createMenus();

signals:

private slots:
    void startCalibrationDialog();
    void startCodeSearchDialog();
    void startSnapshotDialog();
    void startMarkerGeneratorDialog();
    void startMarkerSearchDialog();
    void startSettingsDialog();

public slots:
    void setStatusBarText(const QString &message);
    void writeToTerminal(const QString &message);
    void writeErrorToTerminal(const QString &message);
    void updateImage(const QImage &image);
};

#endif // MAINWINDOW_H
