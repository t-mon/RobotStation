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
    QLabel *m_cameraTransformationLabel;

    void initState();
    void searchState();

    bool m_searchCoordinateSystem;
    bool m_receivePoint;
    QMatrix4x4 m_cameraTransformation;
    QMatrix4x4 m_toolTransformation;


signals:

private slots:
    void nextStep(const int &id);
    void searchPositionButtonClicked();
    void coordinateSystemFound(const QMatrix4x4 &cameraTransformation);
    void robotPointReceived(const QVector3D &translation, const QVector3D &rotation);

public slots:

};

#endif // TOOLCALIBRATIONWIZARD_H
