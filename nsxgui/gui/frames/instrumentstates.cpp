
#include "nsxgui/gui/frames/instrumentstates.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDialogButtonBox>
#include <QGroupBox>
#include <QGridLayout>

InstrumentStates::InstrumentStates()
    : QcrFrame{"instrumentStates"}
{
    setAttribute(Qt::WA_DeleteOnClose, true);
    setWindowFlags(Qt::Window);
    setFrameStyle(QFrame::Box | QFrame::Plain);
    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);

    QVBoxLayout* overallLayout = new QVBoxLayout(this);
    QHBoxLayout* horizLayout = new QHBoxLayout;
    QVBoxLayout* vertical_1 = new QVBoxLayout;
    data = new QListWidget(this);
    vertical_1->addWidget(data);
    QHBoxLayout* hL = new QHBoxLayout;
    hL->addWidget(new QLabel("Frame"));
    frameIndex = new QcrSpinBox("frameIndex", new QcrCell<int>(0), 3);
    hL->addWidget(frameIndex);
    //frameSlider
    vertical_1->addLayout(hL);
    horizLayout->addLayout(vertical_1);
    QVBoxLayout* vertical_2 = new QVBoxLayout;
    vertical_2->addWidget(new QLabel("Refined"));
    QGroupBox* groupBox_1 = new QGroupBox("Sample position", this);
    QHBoxLayout* gbLayout_1 = new QHBoxLayout(groupBox_1);
    gbLayout_1->addWidget(new QLabel("x"));
    samplePosX = new QcrDoubleSpinBox("samplePosX", new QcrCell<double>(0.00), 8, 6);
    gbLayout_1->addWidget(samplePosX);
    gbLayout_1->addWidget(new QLabel("y"));
    samplePosY = new QcrDoubleSpinBox("samplePosY", new QcrCell<double>(0.00), 8, 6);
    gbLayout_1->addWidget(samplePosY);
    gbLayout_1->addWidget(new QLabel("z"));
    samplePosZ = new QcrDoubleSpinBox("samplePosZ", new QcrCell<double>(0.00), 8, 6);
    gbLayout_1->addWidget(samplePosZ);
    vertical_2->addWidget(groupBox_1);
    QGroupBox* sampleOrientation = new QGroupBox("Sample orientation", this);
    QGridLayout* sampleOriGrid = new QGridLayout(sampleOrientation);
    sampleOri_00 = new QcrDoubleSpinBox("sampleOrientation00", new QcrCell<double>(0.00), 8, 6);
    sampleOri_01 = new QcrDoubleSpinBox("sampleOrientation01", new QcrCell<double>(0.10), 8, 6);
    sampleOri_02 = new QcrDoubleSpinBox("sampleOrientation02", new QcrCell<double>(0.20), 8, 6);
    sampleOri_10 = new QcrDoubleSpinBox("sampleOrientation10", new QcrCell<double>(1.00), 8, 6);
    sampleOri_11 = new QcrDoubleSpinBox("sampleOrientation11", new QcrCell<double>(1.10), 8, 6);
    sampleOri_12 = new QcrDoubleSpinBox("sampleOrientation12", new QcrCell<double>(1.20), 8, 6);
    sampleOri_20 = new QcrDoubleSpinBox("sampleOrientation20", new QcrCell<double>(2.00), 8, 6);
    sampleOri_21 = new QcrDoubleSpinBox("sampleOrientation21", new QcrCell<double>(2.10), 8, 6);
    sampleOri_22 = new QcrDoubleSpinBox("sampleOrientation22", new QcrCell<double>(2.20), 8, 6);
    sampleOriGrid->addWidget(sampleOri_00, 0, 0, 1, 1);
    sampleOriGrid->addWidget(sampleOri_01, 0, 1, 1, 1);
    sampleOriGrid->addWidget(sampleOri_02, 0, 2, 1, 1);
    sampleOriGrid->addWidget(sampleOri_10, 1, 0, 1, 1);
    sampleOriGrid->addWidget(sampleOri_11, 1, 1, 1, 1);
    sampleOriGrid->addWidget(sampleOri_12, 1, 2, 1, 1);
    sampleOriGrid->addWidget(sampleOri_20, 2, 0, 1, 1);
    sampleOriGrid->addWidget(sampleOri_21, 2, 1, 1, 1);
    sampleOriGrid->addWidget(sampleOri_22, 2, 2, 1, 1);
    vertical_2->addWidget(sampleOrientation);
    QGroupBox* sampleOffsets = new QGroupBox("Sample orientation offsets", this);
    QGridLayout* offsetGrid = new QGridLayout(sampleOffsets);
    sampleOff_00 = new QcrDoubleSpinBox("sampleOrientationOffset00", new QcrCell<double>(0.00), 8, 6);
    sampleOff_01 = new QcrDoubleSpinBox("sampleOrientationOffset01", new QcrCell<double>(0.10), 8, 6);
    sampleOff_02 = new QcrDoubleSpinBox("sampleOrientationOffset02", new QcrCell<double>(0.20), 8, 6);
    sampleOff_10 = new QcrDoubleSpinBox("sampleOrientationOffset10", new QcrCell<double>(1.00), 8, 6);
    sampleOff_11 = new QcrDoubleSpinBox("sampleOrientationOffset11", new QcrCell<double>(1.10), 8, 6);
    sampleOff_12 = new QcrDoubleSpinBox("sampleOrientationOffset12", new QcrCell<double>(1.20), 8, 6);
    sampleOff_20 = new QcrDoubleSpinBox("sampleOrientationOffset20", new QcrCell<double>(2.00), 8, 6);
    sampleOff_21 = new QcrDoubleSpinBox("sampleOrientationOffset21", new QcrCell<double>(2.10), 8, 6);
    sampleOff_22 = new QcrDoubleSpinBox("sampleOrientationOffset22", new QcrCell<double>(2.20), 8, 6);
    offsetGrid->addWidget(sampleOff_00, 0, 0, 1, 1);
    offsetGrid->addWidget(sampleOff_01, 0, 1, 1, 1);
    offsetGrid->addWidget(sampleOff_02, 0, 2, 1, 1);
    offsetGrid->addWidget(sampleOff_10, 1, 0, 1, 1);
    offsetGrid->addWidget(sampleOff_11, 1, 1, 1, 1);
    offsetGrid->addWidget(sampleOff_12, 1, 2, 1, 1);
    offsetGrid->addWidget(sampleOff_20, 2, 0, 1, 1);
    offsetGrid->addWidget(sampleOff_21, 2, 1, 1, 1);
    offsetGrid->addWidget(sampleOff_22, 2, 2, 1, 1);
    vertical_2->addWidget(sampleOffsets);
    QGroupBox* detectorOrientation = new QGroupBox("Detector orientation", this);
    QGridLayout* detectorGrid = new QGridLayout(detectorOrientation);
    detectorOri_00 = new QcrDoubleSpinBox("detectorOrientation00", new QcrCell<double>(0.00), 8, 6);
    detectorOri_01 = new QcrDoubleSpinBox("detectorOrientation01", new QcrCell<double>(0.10), 8, 6);
    detectorOri_02 = new QcrDoubleSpinBox("detectorOrientation02", new QcrCell<double>(0.20), 8, 6);
    detectorOri_10 = new QcrDoubleSpinBox("detectorOrientation10", new QcrCell<double>(1.00), 8, 6);
    detectorOri_11 = new QcrDoubleSpinBox("detectorOrientation11", new QcrCell<double>(1.10), 8, 6);
    detectorOri_12 = new QcrDoubleSpinBox("detectorOrientation12", new QcrCell<double>(1.20), 8, 6);
    detectorOri_20 = new QcrDoubleSpinBox("detectorOrientation20", new QcrCell<double>(2.00), 8, 6);
    detectorOri_21 = new QcrDoubleSpinBox("detectorOrientation21", new QcrCell<double>(2.10), 8, 6);
    detectorOri_22 = new QcrDoubleSpinBox("detectorOrientation22", new QcrCell<double>(2.20), 8, 6);
    detectorGrid->addWidget(detectorOri_00, 0, 0, 1, 1);
    detectorGrid->addWidget(detectorOri_01, 0, 1, 1, 1);
    detectorGrid->addWidget(detectorOri_02, 0, 2, 1, 1);
    detectorGrid->addWidget(detectorOri_10, 1, 0, 1, 1);
    detectorGrid->addWidget(detectorOri_11, 1, 1, 1, 1);
    detectorGrid->addWidget(detectorOri_12, 1, 2, 1, 1);
    detectorGrid->addWidget(detectorOri_20, 2, 0, 1, 1);
    detectorGrid->addWidget(detectorOri_21, 2, 1, 1, 1);
    detectorGrid->addWidget(detectorOri_22, 2, 2, 1, 1);
    vertical_2->addWidget(detectorOrientation);
    QGroupBox* incommingBeam = new QGroupBox("Normalized incomming beam", this);
    QHBoxLayout* beamBox = new QHBoxLayout(incommingBeam);
    beamBox->addWidget(new QLabel("x"));
    beamX = new QcrDoubleSpinBox("beamX", new QcrCell<double>(0.00), 8, 6);
    beamBox->addWidget(beamX);
    beamBox->addWidget(new QLabel("y"));
    beamY = new QcrDoubleSpinBox("beamY", new QcrCell<double>(0.00), 8, 6);
    beamBox->addWidget(beamY);
    beamBox->addWidget(new QLabel("z"));
    beamZ = new QcrDoubleSpinBox("beamZ", new QcrCell<double>(0.00), 8, 6);
    beamBox->addWidget(beamZ);
    vertical_2->addWidget(incommingBeam);
    horizLayout->addLayout(vertical_2);
    overallLayout->addLayout(horizLayout);
    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok, Qt::Horizontal, this);
    overallLayout->addWidget(buttonBox);

    show();
}
