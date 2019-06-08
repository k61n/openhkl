//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      nsxgui/gui/frames/instrumentstates.cpp
//! @brief     Implements ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************


#include "nsxgui/gui/frames/instrumentstates.h"
#include <QDialogButtonBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QVBoxLayout>

InstrumentStates::InstrumentStates() : QcrFrame {"instrumentStates"}
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
    frameIndex = new QcrSpinBox("adhoc_frameIndex", new QcrCell<int>(0), 3);
    hL->addWidget(frameIndex);
    // frameSlider
    vertical_1->addLayout(hL);
    horizLayout->addLayout(vertical_1);
    QVBoxLayout* vertical_2 = new QVBoxLayout;
    vertical_2->addWidget(new QLabel("Refined"));
    QGroupBox* groupBox_1 = new QGroupBox("Sample position", this);
    QHBoxLayout* gbLayout_1 = new QHBoxLayout(groupBox_1);
    gbLayout_1->addWidget(new QLabel("x"));
    samplePosX = new QcrDoubleSpinBox("adhoc_samplePosX", new QcrCell<double>(0.00), 8, 6);
    gbLayout_1->addWidget(samplePosX);
    gbLayout_1->addWidget(new QLabel("y"));
    samplePosY = new QcrDoubleSpinBox("adhoc_samplePosY", new QcrCell<double>(0.00), 8, 6);
    gbLayout_1->addWidget(samplePosY);
    gbLayout_1->addWidget(new QLabel("z"));
    samplePosZ = new QcrDoubleSpinBox("adhoc_samplePosZ", new QcrCell<double>(0.00), 8, 6);
    gbLayout_1->addWidget(samplePosZ);
    vertical_2->addWidget(groupBox_1);
    QGroupBox* sampleOrientation = new QGroupBox("Sample orientation", this);
    QGridLayout* sampleOriGrid = new QGridLayout(sampleOrientation);
    sampleOri_00 =
        new QcrDoubleSpinBox("adhoc_sampleOrientation00", new QcrCell<double>(0.00), 8, 6);
    sampleOri_01 =
        new QcrDoubleSpinBox("adhoc_sampleOrientation01", new QcrCell<double>(0.10), 8, 6);
    sampleOri_02 =
        new QcrDoubleSpinBox("adhoc_sampleOrientation02", new QcrCell<double>(0.20), 8, 6);
    sampleOri_10 =
        new QcrDoubleSpinBox("adhoc_sampleOrientation10", new QcrCell<double>(1.00), 8, 6);
    sampleOri_11 =
        new QcrDoubleSpinBox("adhoc_sampleOrientation11", new QcrCell<double>(1.10), 8, 6);
    sampleOri_12 =
        new QcrDoubleSpinBox("adhoc_sampleOrientation12", new QcrCell<double>(1.20), 8, 6);
    sampleOri_20 =
        new QcrDoubleSpinBox("adhoc_sampleOrientation20", new QcrCell<double>(2.00), 8, 6);
    sampleOri_21 =
        new QcrDoubleSpinBox("adhoc_sampleOrientation21", new QcrCell<double>(2.10), 8, 6);
    sampleOri_22 =
        new QcrDoubleSpinBox("adhoc_sampleOrientation22", new QcrCell<double>(2.20), 8, 6);
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
    sampleOff_00 =
        new QcrDoubleSpinBox("adhoc_sampleOrientationOffset00", new QcrCell<double>(0.00), 8, 6);
    sampleOff_01 =
        new QcrDoubleSpinBox("adhoc_sampleOrientationOffset01", new QcrCell<double>(0.10), 8, 6);
    sampleOff_02 =
        new QcrDoubleSpinBox("adhoc_sampleOrientationOffset02", new QcrCell<double>(0.20), 8, 6);
    sampleOff_10 =
        new QcrDoubleSpinBox("adhoc_sampleOrientationOffset10", new QcrCell<double>(1.00), 8, 6);
    sampleOff_11 =
        new QcrDoubleSpinBox("adhoc_sampleOrientationOffset11", new QcrCell<double>(1.10), 8, 6);
    sampleOff_12 =
        new QcrDoubleSpinBox("adhoc_sampleOrientationOffset12", new QcrCell<double>(1.20), 8, 6);
    sampleOff_20 =
        new QcrDoubleSpinBox("adhoc_sampleOrientationOffset20", new QcrCell<double>(2.00), 8, 6);
    sampleOff_21 =
        new QcrDoubleSpinBox("adhoc_sampleOrientationOffset21", new QcrCell<double>(2.10), 8, 6);
    sampleOff_22 =
        new QcrDoubleSpinBox("adhoc_sampleOrientationOffset22", new QcrCell<double>(2.20), 8, 6);
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
    detectorOri_00 =
        new QcrDoubleSpinBox("adhoc_detectorOrientation00", new QcrCell<double>(0.00), 8, 6);
    detectorOri_01 =
        new QcrDoubleSpinBox("adhoc_detectorOrientation01", new QcrCell<double>(0.10), 8, 6);
    detectorOri_02 =
        new QcrDoubleSpinBox("adhoc_detectorOrientation02", new QcrCell<double>(0.20), 8, 6);
    detectorOri_10 =
        new QcrDoubleSpinBox("adhoc_detectorOrientation10", new QcrCell<double>(1.00), 8, 6);
    detectorOri_11 =
        new QcrDoubleSpinBox("adhoc_detectorOrientation11", new QcrCell<double>(1.10), 8, 6);
    detectorOri_12 =
        new QcrDoubleSpinBox("adhoc_detectorOrientation12", new QcrCell<double>(1.20), 8, 6);
    detectorOri_20 =
        new QcrDoubleSpinBox("adhoc_detectorOrientation20", new QcrCell<double>(2.00), 8, 6);
    detectorOri_21 =
        new QcrDoubleSpinBox("adhoc_detectorOrientation21", new QcrCell<double>(2.10), 8, 6);
    detectorOri_22 =
        new QcrDoubleSpinBox("adhoc_detectorOrientation22", new QcrCell<double>(2.20), 8, 6);
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
    beamX = new QcrDoubleSpinBox("adhoc_beamX", new QcrCell<double>(0.00), 8, 6);
    beamBox->addWidget(beamX);
    beamBox->addWidget(new QLabel("y"));
    beamY = new QcrDoubleSpinBox("adhoc_beamY", new QcrCell<double>(0.00), 8, 6);
    beamBox->addWidget(beamY);
    beamBox->addWidget(new QLabel("z"));
    beamZ = new QcrDoubleSpinBox("adhoc_beamZ", new QcrCell<double>(0.00), 8, 6);
    beamBox->addWidget(beamZ);
    vertical_2->addWidget(incommingBeam);
    horizLayout->addLayout(vertical_2);
    overallLayout->addLayout(horizLayout);
    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok, Qt::Horizontal, this);
    overallLayout->addWidget(buttonBox);

    show();
}
