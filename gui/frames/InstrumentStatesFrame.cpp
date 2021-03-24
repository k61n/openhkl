//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/frames/InstrumentStatesFrame.cpp
//! @brief     Implements class InstrumentStatesFrame
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "gui/frames/InstrumentStatesFrame.h"

#include "core/data/DataSet.h"
#include "gui/models/Meta.h"
#include "gui/models/Project.h"
#include "gui/models/Session.h"
#include <QDialogButtonBox>
#include <QFileInfo>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QVBoxLayout>

InstrumentStatesFrame::InstrumentStatesFrame() : QFrame()
{
    if (gSession->currentProjectNum() < 0) {
        // gLogger->log("[ERROR] No experiment selected");
        return;
    }
    const std::vector<nsx::sptrDataSet> datalist = gSession->currentProject()->allData();
    if (datalist.empty()) {
        // gLogger->log("[ERROR] No dataset found");
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
    for (const nsx::sptrDataSet& d : datalist) {
        QFileInfo fileinfo(QString::fromStdString(d->filename()));
        QListWidgetItem* item = new QListWidgetItem(fileinfo.baseName());
        item->setData(Qt::UserRole, QVariant::fromValue(d));
        data->addItem(item);
    }
    data->setCurrentRow(0);
    vertical_1->addWidget(data);
    QHBoxLayout* hL = new QHBoxLayout;
    hL->addWidget(new QLabel("Frame"));
    frameIndex = new QSpinBox();
    // "adhoc_frameIndex", new QcrCell<int>(0), 3);
    frameIndex->setReadOnly(true);
    hL->addWidget(frameIndex);
    frameSlider = new QSlider(Qt::Horizontal);
    vertical_1->addLayout(hL);
    vertical_1->addWidget(frameSlider);
    horizLayout->addLayout(vertical_1);
    QVBoxLayout* vertical_2 = new QVBoxLayout;
    refinedLabel = new QLabel("Refined");
    vertical_2->addWidget(refinedLabel);
    QGroupBox* groupBox_1 = new QGroupBox("Sample position", this);
    QHBoxLayout* gbLayout_1 = new QHBoxLayout(groupBox_1);
    gbLayout_1->addWidget(new QLabel("x"));

    samplePosX = new QDoubleSpinBox();
    samplePosX->setReadOnly(true);
    samplePosX->setButtonSymbols(buttonSymbols);
    gbLayout_1->addWidget(samplePosX);

    gbLayout_1->addWidget(new QLabel("y"));

    samplePosY = new QDoubleSpinBox();
    samplePosY->setReadOnly(true);
    samplePosY->setButtonSymbols(buttonSymbols);
    gbLayout_1->addWidget(samplePosY);

    gbLayout_1->addWidget(new QLabel("z"));

    samplePosZ = new QDoubleSpinBox();
    samplePosZ->setReadOnly(true);
    samplePosZ->setButtonSymbols(buttonSymbols);
    gbLayout_1->addWidget(samplePosZ);

    vertical_2->addWidget(groupBox_1);

    QGroupBox* sampleOrientation = new QGroupBox("Sample orientation", this);
    QGridLayout* sampleOriGrid = new QGridLayout(sampleOrientation);

    sampleOri_00 = new QDoubleSpinBox();
    sampleOri_00->setValue(0.00);
    sampleOri_00->setDecimals(8);

    sampleOri_01 = new QDoubleSpinBox();
    sampleOri_01->setValue(0.10);
    sampleOri_01->setDecimals(8);

    sampleOri_02 = new QDoubleSpinBox();
    sampleOri_02->setValue(0.20);
    sampleOri_02->setDecimals(8);

    sampleOri_10 = new QDoubleSpinBox();
    sampleOri_10->setValue(1.00);
    sampleOri_10->setDecimals(8);

    sampleOri_11 = new QDoubleSpinBox();
    sampleOri_11->setValue(1.10);
    sampleOri_11->setDecimals(8);

    sampleOri_12 = new QDoubleSpinBox();
    sampleOri_12->setValue(1.10);
    sampleOri_12->setDecimals(8);

    sampleOri_20 = new QDoubleSpinBox();
    sampleOri_20->setValue(2.00);
    sampleOri_20->setDecimals(8);

    sampleOri_21 = new QDoubleSpinBox();
    sampleOri_21->setValue(2.10);
    sampleOri_21->setDecimals(8);

    sampleOri_22 = new QDoubleSpinBox();
    sampleOri_22->setValue(2.20);
    sampleOri_22->setDecimals(8);

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

    sampleOff_00 = new QDoubleSpinBox();
    sampleOff_00->setValue(0.00);
    sampleOff_00->setDecimals(8);

    sampleOff_01 = new QDoubleSpinBox();
    sampleOff_01->setValue(0.10);
    sampleOff_01->setDecimals(8);

    sampleOff_02 = new QDoubleSpinBox();
    sampleOff_02->setValue(0.20);
    sampleOff_02->setDecimals(8);

    sampleOff_10 = new QDoubleSpinBox();
    sampleOff_10->setValue(1.00);
    sampleOff_10->setDecimals(8);

    sampleOff_11 = new QDoubleSpinBox();
    sampleOff_11->setValue(1.10);
    sampleOff_11->setDecimals(8);

    sampleOff_12 = new QDoubleSpinBox();
    sampleOff_12->setValue(1.10);
    sampleOff_12->setDecimals(8);

    sampleOff_20 = new QDoubleSpinBox();
    sampleOff_20->setValue(2.00);
    sampleOff_20->setDecimals(8);

    sampleOff_21 = new QDoubleSpinBox();
    sampleOff_21->setValue(2.10);
    sampleOff_21->setDecimals(8);

    sampleOff_22 = new QDoubleSpinBox();
    sampleOff_22->setValue(2.20);
    sampleOff_22->setDecimals(8);

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

    detectorOri_00 = new QDoubleSpinBox();
    detectorOri_00->setValue(0.00);
    detectorOri_00->setDecimals(8);

    detectorOri_01 = new QDoubleSpinBox();
    detectorOri_01->setValue(0.10);
    detectorOri_01->setDecimals(8);

    detectorOri_02 = new QDoubleSpinBox();
    detectorOri_02->setValue(0.20);
    detectorOri_02->setDecimals(8);

    detectorOri_10 = new QDoubleSpinBox();
    detectorOri_10->setValue(1.00);
    detectorOri_10->setDecimals(8);

    detectorOri_11 = new QDoubleSpinBox();
    detectorOri_11->setValue(1.10);
    detectorOri_11->setDecimals(8);

    detectorOri_12 = new QDoubleSpinBox();
    detectorOri_12->setValue(1.10);
    detectorOri_12->setDecimals(8);

    detectorOri_20 = new QDoubleSpinBox();
    detectorOri_20->setValue(2.00);
    detectorOri_20->setDecimals(8);

    detectorOri_21 = new QDoubleSpinBox();
    detectorOri_21->setValue(2.10);
    detectorOri_21->setDecimals(8);

    detectorOri_22 = new QDoubleSpinBox();
    detectorOri_22->setValue(2.20);
    detectorOri_22->setDecimals(8);

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
    beamX = new QDoubleSpinBox();
    beamX->setReadOnly(true);
    beamX->setButtonSymbols(buttonSymbols);
    beamBox->addWidget(beamX);

    beamBox->addWidget(new QLabel("y"));
    beamY = new QDoubleSpinBox();
    beamY->setReadOnly(true);
    beamY->setButtonSymbols(buttonSymbols);
    beamBox->addWidget(beamY);

    beamBox->addWidget(new QLabel("z"));
    beamZ = new QDoubleSpinBox();
    beamZ->setReadOnly(true);
    beamZ->setButtonSymbols(buttonSymbols);
    beamBox->addWidget(beamZ);

    vertical_2->addWidget(incommingBeam);
    horizLayout->addLayout(vertical_2);
    overallLayout->addLayout(horizLayout);
    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok, Qt::Horizontal, this);
    overallLayout->addWidget(buttonBox);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &InstrumentStatesFrame::close);
    connect(frameSlider, &QSlider::valueChanged, [=](int i) {
        selectedFrameChanged(i);
        frameIndex->setValue(i);
    });
    connect(
        data, &QListWidget::currentRowChanged, this, &InstrumentStatesFrame::selectedDataChanged);
    selectedDataChanged(0);
    show();
}

void InstrumentStatesFrame::selectedDataChanged(int selectedData)
{
    Q_UNUSED(selectedData)
    QListWidgetItem* currentItem = data->currentItem();
    nsx::sptrDataSet currentData = currentItem->data(Qt::UserRole).value<nsx::sptrDataSet>();
    frameIndex->setValue(0);
    frameSlider->setMinimum(0);
    frameSlider->setMaximum(currentData->nFrames() - 1);
    frameSlider->setValue(0);
    selectedFrameChanged(0);
}

void InstrumentStatesFrame::selectedFrameChanged(int selectedFrame)
{
    nsx::sptrDataSet currentData =
        data->currentItem()->data(Qt::UserRole).value<nsx::sptrDataSet>();
    const nsx::InstrumentStateList& instrumentStates = currentData->instrumentStates();
    nsx::InstrumentState selectedState = instrumentStates[selectedFrame];

    QFont font;
    font.setBold(true);
    refinedLabel->setStyleSheet(
        selectedState.refined ? "QLabel {color : blue;}" : "QLabel {color : red;}");
    refinedLabel->setFont(font);
    refinedLabel->setText(selectedState.refined ? "Refined" : "Not refined");

    const auto& samplePosition = selectedState.samplePosition;
    samplePosX->setValue(samplePosition[0]);
    samplePosY->setValue(samplePosition[1]);
    samplePosZ->setValue(samplePosition[2]);

    const auto& sampleOri = selectedState.sampleOrientation.normalized().toRotationMatrix();
    sampleOri_00->setValue(sampleOri(0, 0));
    sampleOri_01->setValue(sampleOri(0, 1));
    sampleOri_02->setValue(sampleOri(0, 2));
    sampleOri_10->setValue(sampleOri(1, 0));
    sampleOri_11->setValue(sampleOri(1, 1));
    sampleOri_12->setValue(sampleOri(1, 2));
    sampleOri_20->setValue(sampleOri(2, 0));
    sampleOri_21->setValue(sampleOri(2, 1));
    sampleOri_22->setValue(sampleOri(2, 2));

    const auto& sampleOffset =
        selectedState.sampleOrientationOffset.normalized().toRotationMatrix();
    sampleOff_00->setValue(sampleOffset(0, 0));
    sampleOff_01->setValue(sampleOffset(0, 1));
    sampleOff_02->setValue(sampleOffset(0, 2));
    sampleOff_10->setValue(sampleOffset(1, 0));
    sampleOff_11->setValue(sampleOffset(1, 1));
    sampleOff_12->setValue(sampleOffset(1, 2));
    sampleOff_20->setValue(sampleOffset(2, 0));
    sampleOff_21->setValue(sampleOffset(2, 1));
    sampleOff_22->setValue(sampleOffset(2, 2));

    const auto& detectorOri = selectedState.detectorOrientation;
    detectorOri_00->setValue(detectorOri(0, 0));
    detectorOri_01->setValue(detectorOri(0, 1));
    detectorOri_02->setValue(detectorOri(0, 2));
    detectorOri_10->setValue(detectorOri(1, 0));
    detectorOri_11->setValue(detectorOri(1, 1));
    detectorOri_12->setValue(detectorOri(1, 2));
    detectorOri_20->setValue(detectorOri(2, 0));
    detectorOri_21->setValue(detectorOri(2, 1));
    detectorOri_22->setValue(detectorOri(2, 2));

    const auto& ni = selectedState.ni;
    beamX->setValue(ni[0]);
    beamY->setValue(ni[1]);
    beamZ->setValue(ni[2]);
}
