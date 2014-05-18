#ifndef TOOLCALIBRATIONWIZARD_H
#define TOOLCALIBRATIONWIZARD_H

#include <QWizard>
#include <QLabel>
#include <QVBoxLayout>
#include <QWizardPage>
#include <QLabel>
#include <QDebug>
#include <QMatrix4x4>

class ToolCalibrationWizard : public QWizard
{
    Q_OBJECT
public:
    explicit ToolCalibrationWizard();
    ~ToolCalibrationWizard();

private:
    QWizardPage *createIntroPage();
    QWizardPage *createSearchPage();
    QWizardPage *createNotConnectedPage();
    QWizardPage *createPlaceCoordinateSystemPage();
    QWizardPage *createFoundCoordinateSystemPage();
    QWizardPage *createTeachCoordinateSystemPage();
    QWizardPage *createTeachSystemFinishedPage();
    QWizardPage *createGetTcpPositionPage();
    QWizardPage *createCalculateOffsetPage();

    QLabel *m_cameraTransformationLabel;
    QLabel *m_toolPointLabel;
    QLabel *m_offsetTransformationLabel;


    bool m_searchCoordinateSystem;
    bool m_receivePoint;
    QMatrix4x4 m_cameraTransformation;
    QMatrix4x4 m_toolTransformation;

    QMatrix4x4 m_offsetTransformation;

    float roundValue(float value);
    void calculateOffset();
    void saveOffset();

signals:

private slots:
    void nextStep(const int &id);
    void searchPositionButtonClicked();
    void requestPointButtonClicked();
    void coordinateSystemFound(const QMatrix4x4 &cameraTransformation);
    void robotPointReceived(const QVector3D &translation, const QVector3D &rotation);

public slots:

};

#endif // TOOLCALIBRATIONWIZARD_H
