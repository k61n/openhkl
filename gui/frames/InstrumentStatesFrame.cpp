//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/frames/InstrumentStatesFrame.cpp
//! @brief     Implements class InstrumentStates
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************


#include "gui/frames/InstrumentStatesFrame.h"

#include "gui/models/Session.h"
#include "gui/models/Meta.h"
#include "core/experiment/DataSet.h"
#include <QDialogButtonBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFileInfo>

InstrumentStates::InstrumentStates() : QcrFrame {"instrumentStates"}
{
    if (gSession->selectedExperimentNum() < 0) {
        gLogger->log("[ERROR] No experiment selected");
        return;
    }
    QList<nsx::sptrDataSet> datalist = gSession->selectedExperiment()->data()->allData();
    if (datalist.empty()) {
        gLogger->log("[ERROR] No dataset found");
        return;
    }
    QAbstractSpinBox::ButtonSymbols buttonSymbols = QAbstractSpinBox::NoButtons;
    setAttribute(Qt::WA_DeleteOnClose, true);
    setWindowFlags(Qt::Window);
    setFrameStyle(QFrame::Box | QFrame::Plain);
    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);

    QVBoxLayout* overallLayout = new QVBoxLayout(this);
    QHBoxLayout* horizLayout = new QHBoxLayout;
    QVBoxLayout* vertical_1 = new QVBoxLayout;
    data = new QListWidget(this);
    for (nsx::sptrDataSet d : datalist) {
        QFileInfo fileinfo(QString::fromStdString(d->filename()));
        QListWidgetItem* item = new QListWidgetItem(fileinfo.baseName());
        item->setData(Qt::UserRole, QVariant::fromValue(d));
        data->addItem(item);
    }
    data->setCurrentRow(0);
    vertical_1->addWidget(data);
    QHBoxLayout* hL = new QHBoxLayout;
    hL->addWidget(new QLabel("Frame"));
    frameIndex = new QcrSpinBox("adhoc_frameIndex", new QcrCell<int>(0), 3);
    hL->addWidget(frameIndex);
    // frameSlider
    vertical_1->addLayout(hL);
    horizLayout->addLayout(vertical_1);
    QVBoxLayout* vertical_2 = new QVBoxLayout;
    refinedLabel = new QLabel("Refined");
    vertical_2->addWidget(refinedLabel);
    QGroupBox* groupBox_1 = new QGroupBox("Sample position", this);
    QHBoxLayout* gbLayout_1 = new QHBoxLayout(groupBox_1);
    gbLayout_1->addWidget(new QLabel("x"));
    samplePosX = new QcrDoubleSpinBox("adhoc_samplePosX", new QcrCell<double>(0.00), 8, 6);
    samplePosX->setReadOnly(true);
    samplePosX->setButtonSymbols(buttonSymbols);
    gbLayout_1->addWidget(samplePosX);
    gbLayout_1->addWidget(new QLabel("y"));
    samplePosY = new QcrDoubleSpinBox("adhoc_samplePosY", new QcrCell<double>(0.00), 8, 6);
    samplePosY->setReadOnly(true);
    samplePosY->setButtonSymbols(buttonSymbols);
    gbLayout_1->addWidget(samplePosY);
    gbLayout_1->addWidget(new QLabel("z"));
    samplePosZ = new QcrDoubleSpinBox("adhoc_samplePosZ", new QcrCell<double>(0.00), 8, 6);
    samplePosZ->setReadOnly(true);
    samplePosZ->setButtonSymbols(buttonSymbols);
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
    sampleOri_00->setReadOnly(true);
    sampleOri_00->setButtonSymbols(buttonSymbols);
    sampleOri_01->setReadOnly(true);
    sampleOri_01->setButtonSymbols(buttonSymbols);
    sampleOri_02->setReadOnly(true);
    sampleOri_02->setButtonSymbols(buttonSymbols);
    sampleOri_10->setReadOnly(true);
    sampleOri_10->setButtonSymbols(buttonSymbols);
    sampleOri_11->setReadOnly(true);
    sampleOri_11->setButtonSymbols(buttonSymbols);
    sampleOri_12->setReadOnly(true);
    sampleOri_12->setButtonSymbols(buttonSymbols);
    sampleOri_20->setReadOnly(true);
    sampleOri_20->setButtonSymbols(buttonSymbols);
    sampleOri_21->setReadOnly(true);
    sampleOri_21->setButtonSymbols(buttonSymbols);
    sampleOri_22->setReadOnly(true);
    sampleOri_22->setButtonSymbols(buttonSymbols);
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
    sampleOff_00->setReadOnly(true);
    sampleOff_00->setButtonSymbols(buttonSymbols);
    sampleOff_01->setReadOnly(true);
    sampleOff_01->setButtonSymbols(buttonSymbols);
    sampleOff_02->setReadOnly(true);
    sampleOff_02->setButtonSymbols(buttonSymbols);
    sampleOff_10->setReadOnly(true);
    sampleOff_10->setButtonSymbols(buttonSymbols);
    sampleOff_11->setReadOnly(true);
    sampleOff_11->setButtonSymbols(buttonSymbols);
    sampleOff_12->setReadOnly(true);
    sampleOff_12->setButtonSymbols(buttonSymbols);
    sampleOff_20->setReadOnly(true);
    sampleOff_20->setButtonSymbols(buttonSymbols);
    sampleOff_21->setReadOnly(true);
    sampleOff_21->setButtonSymbols(buttonSymbols);
    sampleOff_22->setReadOnly(true);
    sampleOff_22->setButtonSymbols(buttonSymbols);
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
    detectorOri_00->setReadOnly(true);
    detectorOri_00->setButtonSymbols(buttonSymbols);
    detectorOri_01->setReadOnly(true);
    detectorOri_01->setButtonSymbols(buttonSymbols);
    detectorOri_02->setReadOnly(true);
    detectorOri_02->setButtonSymbols(buttonSymbols);
    detectorOri_10->setReadOnly(true);
    detectorOri_10->setButtonSymbols(buttonSymbols);
    detectorOri_11->setReadOnly(true);
    detectorOri_11->setButtonSymbols(buttonSymbols);
    detectorOri_12->setReadOnly(true);
    detectorOri_12->setButtonSymbols(buttonSymbols);
    detectorOri_20->setReadOnly(true);
    detectorOri_20->setButtonSymbols(buttonSymbols);
    detectorOri_21->setReadOnly(true);
    detectorOri_21->setButtonSymbols(buttonSymbols);
    detectorOri_22->setReadOnly(true);
    detectorOri_22->setButtonSymbols(buttonSymbols);
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
    beamX->setReadOnly(true);
    beamX->setButtonSymbols(buttonSymbols);
    beamBox->addWidget(beamX);
    beamBox->addWidget(new QLabel("y"));
    beamY = new QcrDoubleSpinBox("adhoc_beamY", new QcrCell<double>(0.00), 8, 6);
    beamY->setReadOnly(true);
    beamY->setButtonSymbols(buttonSymbols);
    beamBox->addWidget(beamY);
    beamBox->addWidget(new QLabel("z"));
    beamZ = new QcrDoubleSpinBox("adhoc_beamZ", new QcrCell<double>(0.00), 8, 6);
    beamZ->setReadOnly(true);
    beamZ->setButtonSymbols(buttonSymbols);
    beamBox->addWidget(beamZ);
    vertical_2->addWidget(incommingBeam);
    horizLayout->addLayout(vertical_2);
    overallLayout->addLayout(horizLayout);
    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok, Qt::Horizontal, this);
    overallLayout->addWidget(buttonBox);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &InstrumentStates::close);
    frameIndex->setHook([=](int i){ selectedFrameChanged(i); });
    connect(data, &QListWidget::currentRowChanged, this, &InstrumentStates::selectedDataChanged);
    selectedFrameChanged(0);
    show();
}

void InstrumentStates::selectedDataChanged(int selectedData)
{
    Q_UNUSED(selectedData)
    QListWidgetItem* currentItem = data->currentItem();
    nsx::sptrDataSet currentData = currentItem->data(Qt::UserRole).value<nsx::sptrDataSet>();
    frameIndex->setMinimum(0);
    frameIndex->setMaximum(currentData->nFrames() - 1);
    frameIndex->setCellValue(0);
    selectedFrameChanged(0);
}

void InstrumentStates::selectedFrameChanged(int selectedFrame)
{
    nsx::sptrDataSet currentData = data->currentItem()->data(Qt::UserRole).value<nsx::sptrDataSet>();
    const nsx::InstrumentStateList& instrumentStates = currentData->instrumentStates();
    nsx::InstrumentState selectedState = instrumentStates[selectedFrame];

    QFont font;
    font.setBold(true);
    refinedLabel->setStyleSheet(
                selectedState.refined ? "QLabel {color : blue;}" : "QLabel {color : red;}");
    refinedLabel->setFont(font);
    refinedLabel->setText(selectedState.refined ? "Refined" : "Not refined");

    const auto& samplePosition = selectedState.samplePosition;
    samplePosX->setCellValue(samplePosition[0]);
    samplePosY->setCellValue(samplePosition[1]);
    samplePosZ->setCellValue(samplePosition[2]);

    const auto& sampleOri = selectedState.sampleOrientation.normalized().toRotationMatrix();
    sampleOri_00->setCellValue(sampleOri(0,0));
    sampleOri_01->setCellValue(sampleOri(0,1));
    sampleOri_02->setCellValue(sampleOri(0,2));
    sampleOri_10->setCellValue(sampleOri(1,0));
    sampleOri_11->setCellValue(sampleOri(1,1));
    sampleOri_12->setCellValue(sampleOri(1,2));
    sampleOri_20->setCellValue(sampleOri(2,0));
    sampleOri_21->setCellValue(sampleOri(2,1));
    sampleOri_22->setCellValue(sampleOri(2,2));

    const auto& sampleOffset = selectedState.sampleOrientationOffset.normalized().toRotationMatrix();
    sampleOff_00->setCellValue(sampleOffset(0,0));
    sampleOff_01->setCellValue(sampleOffset(0,1));
    sampleOff_02->setCellValue(sampleOffset(0,2));
    sampleOff_10->setCellValue(sampleOffset(1,0));
    sampleOff_11->setCellValue(sampleOffset(1,1));
    sampleOff_12->setCellValue(sampleOffset(1,2));
    sampleOff_20->setCellValue(sampleOffset(2,0));
    sampleOff_21->setCellValue(sampleOffset(2,1));
    sampleOff_22->setCellValue(sampleOffset(2,2));

    const auto& detectorOri = selectedState.detectorOrientation;
    detectorOri_00->setCellValue(detectorOri(0,0));
    detectorOri_01->setCellValue(detectorOri(0,1));
    detectorOri_02->setCellValue(detectorOri(0,2));
    detectorOri_10->setCellValue(detectorOri(1,0));
    detectorOri_11->setCellValue(detectorOri(1,1));
    detectorOri_12->setCellValue(detectorOri(1,2));
    detectorOri_20->setCellValue(detectorOri(2,0));
    detectorOri_21->setCellValue(detectorOri(2,1));
    detectorOri_22->setCellValue(detectorOri(2,2));

    const auto& ni = selectedState.ni;
    beamX->setCellValue(ni[0]);
    beamY->setCellValue(ni[1]);
    beamZ->setCellValue(ni[2]);
}
