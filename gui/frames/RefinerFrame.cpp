//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/frames/RefinerFrame.cpp
//! @brief     Implements classes Refiner, RefinerFitWidget
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "gui/frames/RefinerFrame.h"

#include "gui/models/Session.h"
#include "gui/models/Meta.h"
#include "core/peak/Peak3D.h"
#include "core/experiment/DataSet.h"
#include "core/experiment/DataTypes.h"
#include "core/peak/Peak3D.h"
#include "core/algo/Refiner.h"
#include <QCR/engine/logger.h>
#include <QFileInfo>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QSpacerItem>
#include <QVBoxLayout>

Refiner::Refiner() : QcrFrame {"RefinerFrame"}
{
    if (gSession->selectedExperimentNum() < 0) {
        gLogger->log("[ERROR] No experiment selected");
        return;
    }

    if (gSession->selectedExperiment()->peaks()->allPeaks().empty()) {
        gLogger->log("[ERROR] No peaks in selected experiment");
        return;
    }

    setAttribute(Qt::WA_DeleteOnClose);
    layout();
}

void Refiner::layout()
{
    tabs = new QcrTabWidget("adhoc_refinerTabs");
    settings = new QcrWidget("adhoc_refinerSettings");
    QVBoxLayout* layoutSettings = new QVBoxLayout(settings);
    PeaksTableModel* model = new PeaksTableModel(
        "adhoc_refinerPeaks", gSession->selectedExperiment()->experiment(),
        gSession->selectedExperiment()->peaks()->allPeaks());
    peaks = new PeaksTableView;
    peaks->setModel(model);
    peaks->selectAll();
    layoutSettings->addWidget(peaks);
    QGroupBox* params = new QGroupBox("Refinable parameters");
    QVBoxLayout* layoutParams = new QVBoxLayout(params);
    refine_lattice =
        new QcrCheckBox("adhoc_refineLattice", "lattice parameters (UB)", new QcrCell<bool>(true));
    refine_samplePosition =
        new QcrCheckBox("adhoc_refineSamplePosition", "Sample position", new QcrCell<bool>(true));
    refine_detectorPosition = new QcrCheckBox(
        "adhoc_refineDetectorPosition", "Detector position", new QcrCell<bool>(false));
    refine_sampleOrientation = new QcrCheckBox(
        "adhoc_refineSampleOrientation", "Sample orientation", new QcrCell<bool>(true));
    refine_ki = new QcrCheckBox("adhoc_refineKi", "ki", new QcrCell<bool>(true));
    layoutParams->addWidget(refine_lattice);
    layoutParams->addWidget(refine_samplePosition);
    layoutParams->addWidget(refine_detectorPosition);
    layoutParams->addWidget(refine_sampleOrientation);
    layoutParams->addWidget(refine_ki);
    layoutSettings->addWidget(params);
    QHBoxLayout* layoutLine = new QHBoxLayout;
    layoutLine->addWidget(new QLabel("Number of batches"));
    numberBatches = new QcrSpinBox("adhoc_numberBatches", new QcrCell<int>(1), 6);
    layoutLine->addWidget(numberBatches);
    layoutLine->addItem(new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));
    layoutSettings->addLayout(layoutLine);
    tabs->addTab(settings, "Settings");
    QVBoxLayout* layoutRefiner = new QVBoxLayout(this);
    layoutRefiner->addWidget(tabs);
    buttons = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel | QDialogButtonBox::Apply, Qt::Horizontal);
    layoutRefiner->addWidget(buttons);

    connect(tabs, &QcrTabWidget::tabCloseRequested, this, &Refiner::tabRemoved);
    connect(buttons, &QDialogButtonBox::clicked, this, &Refiner::actionClicked);

    show();
}

void Refiner::actionClicked(QAbstractButton* button)
{
    auto button_role = buttons->standardButton(button);

    switch (button_role) {
    case QDialogButtonBox::StandardButton::Apply: {
        refine();
        break;
    }
    case QDialogButtonBox::StandardButton::Cancel: {
        close();
        break;
    }
    case QDialogButtonBox::StandardButton::Ok: {
        accept();
        break;
    }
    default: {
        return;
    }
    }
}

void Refiner::tabRemoved(int index)
{
    RefinerFitWidget* refiner_fit_tab = dynamic_cast<RefinerFitWidget*>(tabs->widget(index));
    if (!refiner_fit_tab)
        return;
    tabs->removeTab(index);
    delete refiner_fit_tab;
}

void Refiner::refine()
{
    // Check and construct the peak selection
    QItemSelectionModel* selection_model = peaks->selectionModel();
    QModelIndexList selected_rows = selection_model->selectedRows();
    if (selected_rows.size() < 100) {
        gLogger->log("[ERROR] No or not enough peaks selected for refining");
        return;
    }

    PeaksTableModel* peaks_model = dynamic_cast<PeaksTableModel*>(peaks->model());
    nsx::PeakList allPeaks = peaks_model->peaks();
    nsx::PeakList selected_peaks;
    for (QModelIndex r : selected_rows)
        selected_peaks.push_back(allPeaks[r.row()]);

    nsx::sptrUnitCell unit_cell = selected_peaks[0]->unitCell();

    if (!unit_cell) {
        gLogger->log("[ERROR] No unit cell set for the selected peaks");
        return;
    }

    int n_batches = numberBatches->value();

    std::map<nsx::sptrDataSet, nsx::Refiner> refiners;

    std::set<nsx::sptrDataSet> data;
    // get list of datasets
    for (nsx::sptrPeak3D p : allPeaks)
        data.insert(p->data());

    for (nsx::sptrDataSet d : data) {

        nsx::PeakList reference_peaks, predicted_peaks;

        // Keep the peak that belong to this data and split them between the found
        // and predicted ones
        for (nsx::sptrPeak3D peak : selected_peaks) {
            if (peak->data() != d)
                continue;
            if (peak->predicted())
                predicted_peaks.push_back(peak);
            else
                reference_peaks.push_back(peak);
        }

        gLogger->log(
            "[INFO] " + QString::number(reference_peaks.size()) + " splitted into "
            + QString::number(n_batches) + "refining batches.");

        std::vector<nsx::InstrumentState>& states = d->instrumentStates();

        nsx::Refiner refiner(states, unit_cell, reference_peaks, n_batches);

        if (refine_lattice->isChecked()) {
            refiner.refineUB();
            gLogger->log("[INFO] Refining UB matrix");
        }

        if (refine_samplePosition->isChecked()) {
            refiner.refineSamplePosition();
            gLogger->log("[INFO] Refinining sample position");
        }

        if (refine_detectorPosition->isChecked()) {
            refiner.refineDetectorOffset();
            gLogger->log("[INFO] Refinining detector position");
        }

        if (refine_sampleOrientation->isChecked()) {
            refiner.refineSampleOrientation();
            gLogger->log("[INFO] Refinining sample orientation");
        }

        if (refine_ki->isChecked()) {
            refiner.refineKi();
            gLogger->log("[INFO] Refining Ki");
        }

        bool success = refiner.refine();

        if (success) {
            gLogger->log(
                "[INFO] Successfully refined parameters for numor "
                + QString::fromStdString(d->filename()));
            int updated = refiner.updatePredictions(predicted_peaks);
            refiners.emplace(d, std::move(refiner));
            gLogger->log("[INFO] done; updated " + QString::number(updated) + " peaks");
        } else {
            gLogger->log(
                "[INFO] Failed to refine parameters for numor "
                + QString::fromStdString(d->filename()));
        }
    }

    if (!refiners.empty()) {
        if (tabs->count() == 2) {
            QWidget* refiner_fit_tab = tabs->widget(1);
            tabs->removeTab(1);
            delete refiner_fit_tab;
        }
        RefinerFitWidget* refiner_fit_tab = new RefinerFitWidget(refiners);
        tabs->addTab(refiner_fit_tab, "Fit");
    }

    // Update the peak table view
    QModelIndex topLeft = peaks_model->index(0, 0);
    QModelIndex bottomRight =
        peaks_model->index(peaks_model->rowCount() - 1, peaks_model->columnCount() - 1);
    emit peaks_model->dataChanged(topLeft, bottomRight);
}

void Refiner::accept()
{
    //    auto peaks_item = _experiment_item->peaksItem();
    //    emit _experiment_item->model()->itemChanged(peaks_item);
    close();
}

//  ***********************************************************************************************

RefinerFitWidget::RefinerFitWidget(const std::map<nsx::sptrDataSet, nsx::Refiner>& refiners)
    : QcrWidget {"adhoc_refinerWidgetTab"}, _refiners {refiners}
{
    layout();

    for (auto p : refiners) {
        nsx::sptrDataSet data = p.first;
        QFileInfo fileinfo(QString::fromStdString(data->filename()));

        QListWidgetItem* item = new QListWidgetItem(fileinfo.baseName());
        item->setData(Qt::UserRole, QVariant::fromValue(data));
        selectedData->addItem(item);
    }

    connect(
        selectedData, &QListWidget::currentRowChanged, this,
        &RefinerFitWidget::selectedDataChanged);

    connect(
        batch, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this,
        &RefinerFitWidget::selectedBatchChanged);

    connect(
        frame, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this,
        &RefinerFitWidget::selectedFrameChanged);
    //    connect(slider, &QSlider::valueChanged, this,
    //            &RefinerFitWidget::selectedFrameChanged); <-Slider
}

void RefinerFitWidget::layout()
{
    QVBoxLayout* whole = new QVBoxLayout(this);
    QHBoxLayout* above = new QHBoxLayout;
    QGroupBox* datagroup = new QGroupBox("Data");
    QVBoxLayout* datalayout = new QVBoxLayout(datagroup);
    selectedData = new QListWidget;
    datalayout->addWidget(selectedData);
    QHBoxLayout* dataLine = new QHBoxLayout;
    dataLine->addWidget(new QLabel("Batch"));
    batch = new QcrSpinBox("adhoc_refinerFitBatch", new QcrCell<int>(0), 6);
    dataLine->addWidget(batch);
    dataLine->addItem(new QSpacerItem(20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));
    datalayout->addLayout(dataLine);
    above->addWidget(datagroup);
    above->addWidget(new QTreeView);
    whole->addLayout(above);
    QLabel* boxlabel = new QLabel("Refined");
    boxlabel->setFrameShape(QLabel::Box);
    whole->addWidget(boxlabel);
    QHBoxLayout* middle = new QHBoxLayout;
    QVBoxLayout* leftmiddle = new QVBoxLayout;
    QGroupBox* samplePosition = new QGroupBox("Sample position");
    QHBoxLayout* samplePosLayout = new QHBoxLayout(samplePosition);
    samplePosX = new QcrDoubleSpinBox("adhoc_refinedSamplePosX", new QcrCell<double>(0.0), 10, 6);
    samplePosY = new QcrDoubleSpinBox("adhoc_refinedSamplePosY", new QcrCell<double>(0.0), 10, 6);
    samplePosZ = new QcrDoubleSpinBox("adhoc_refinedSamplePosZ", new QcrCell<double>(0.0), 10, 6);
    samplePosLayout->addWidget(samplePosX);
    samplePosLayout->addWidget(samplePosY);
    samplePosLayout->addWidget(samplePosZ);
    leftmiddle->addWidget(samplePosition);
    QGroupBox* detectorPosition = new QGroupBox("Detector position offsets");
    QHBoxLayout* detectorPosLayout = new QHBoxLayout(detectorPosition);
    detectorPosX =
        new QcrDoubleSpinBox("adhoc_refinedDetectorPosX", new QcrCell<double>(0.0), 10, 6);
    detectorPosY =
        new QcrDoubleSpinBox("adhoc_refinedDetectorPosY", new QcrCell<double>(0.0), 10, 6);
    detectorPosZ =
        new QcrDoubleSpinBox("adhoc_refinedDetectorPosZ", new QcrCell<double>(0.0), 10, 6);
    // lauft bis hier mindestens
    detectorPosLayout->addWidget(detectorPosX);
    detectorPosLayout->addWidget(detectorPosY);
    detectorPosLayout->addWidget(detectorPosZ);
    leftmiddle->addWidget(detectorPosition);
    QGroupBox* incident = new QGroupBox("Incident beam");
    QGridLayout* incidentGrid = new QGridLayout(incident);
    incidentGrid->addWidget(new QLabel("ni"), 0, 0, 1, 1);
    incidentGrid->addWidget(new QLabel("wavelength"), 1, 0, 1, 1);
    niX = new QcrDoubleSpinBox("adhoc_refinedNiX", new QcrCell<double>(0.0), 10, 6);
    niY = new QcrDoubleSpinBox("adhoc_refinedNiY", new QcrCell<double>(0.0), 10, 6);
    niZ = new QcrDoubleSpinBox("adhoc_refinedNiZ", new QcrCell<double>(0.0), 10, 6);
    wavelength = new QcrDoubleSpinBox("adhoc_refinedwavelength", new QcrCell<double>(0.0), 10, 6);
    wavelengthOffset =
        new QcrDoubleSpinBox("adhoc_refinedwavelengthOffset", new QcrCell<double>(0.0), 10, 6);
    incidentGrid->addWidget(niX, 0, 1, 1, 1);
    incidentGrid->addWidget(niY, 0, 2, 1, 1);
    incidentGrid->addWidget(niZ, 0, 3, 1, 1);
    incidentGrid->addWidget(wavelength, 1, 1, 1, 1);
    incidentGrid->addWidget(wavelengthOffset, 1, 2, 1, 1);
    leftmiddle->addWidget(incident);
    leftmiddle->addItem(new QSpacerItem(20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding));
    middle->addLayout(leftmiddle);
    QVBoxLayout* midmiddle = new QVBoxLayout;
    QGroupBox* sampleOrientation = new QGroupBox("Sample orientation");
    QGridLayout* sampleGrid = new QGridLayout(sampleOrientation);
    sampleOrientation00 =
        new QcrDoubleSpinBox("adhoc_refinedSampleOri00", new QcrCell<double>(0.0), 10, 6);
    sampleOrientation01 =
        new QcrDoubleSpinBox("adhoc_refinedSampleOri01", new QcrCell<double>(0.0), 10, 6);
    sampleOrientation02 =
        new QcrDoubleSpinBox("adhoc_refinedSampleOri02", new QcrCell<double>(0.0), 10, 6);
    sampleOrientation10 =
        new QcrDoubleSpinBox("adhoc_refinedSampleOri10", new QcrCell<double>(0.0), 10, 6);
    sampleOrientation11 =
        new QcrDoubleSpinBox("adhoc_refinedSampleOri11", new QcrCell<double>(0.0), 10, 6);
    sampleOrientation12 =
        new QcrDoubleSpinBox("adhoc_refinedSampleOri12", new QcrCell<double>(0.0), 10, 6);
    sampleOrientation20 =
        new QcrDoubleSpinBox("adhoc_refinedSampleOri20", new QcrCell<double>(0.0), 10, 6);
    sampleOrientation21 =
        new QcrDoubleSpinBox("adhoc_refinedSampleOri21", new QcrCell<double>(0.0), 10, 6);
    sampleOrientation22 =
        new QcrDoubleSpinBox("adhoc_refinedSampleOri22", new QcrCell<double>(0.0), 10, 6);
    sampleGrid->addWidget(sampleOrientation00, 0, 0, 1, 1);
    sampleGrid->addWidget(sampleOrientation01, 0, 1, 1, 1);
    sampleGrid->addWidget(sampleOrientation02, 0, 2, 1, 1);
    sampleGrid->addWidget(sampleOrientation10, 1, 0, 1, 1);
    sampleGrid->addWidget(sampleOrientation11, 1, 1, 1, 1);
    sampleGrid->addWidget(sampleOrientation12, 1, 2, 1, 1);
    sampleGrid->addWidget(sampleOrientation20, 2, 0, 1, 1);
    sampleGrid->addWidget(sampleOrientation21, 2, 1, 1, 1);
    sampleGrid->addWidget(sampleOrientation22, 2, 2, 1, 1);
    midmiddle->addWidget(sampleOrientation);
    midmiddle->addItem(new QSpacerItem(20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding));
    middle->addLayout(midmiddle);
    QVBoxLayout* rightmiddle = new QVBoxLayout;
    QGroupBox* detectorOrientation = new QGroupBox("Detector orientation");
    QGridLayout* detectorGrid = new QGridLayout(detectorOrientation);
    detectorOrientation00 =
        new QcrDoubleSpinBox("adhoc_refinedDetectorOri00", new QcrCell<double>(0.0), 10, 6);
    detectorOrientation01 =
        new QcrDoubleSpinBox("adhoc_refinedDetectorOri01", new QcrCell<double>(0.0), 10, 6);
    detectorOrientation02 =
        new QcrDoubleSpinBox("adhoc_refinedDetectorOri02", new QcrCell<double>(0.0), 10, 6);
    detectorOrientation10 =
        new QcrDoubleSpinBox("adhoc_refinedDetectorOri10", new QcrCell<double>(0.0), 10, 6);
    detectorOrientation11 =
        new QcrDoubleSpinBox("adhoc_refinedDetectorOri11", new QcrCell<double>(0.0), 10, 6);
    detectorOrientation12 =
        new QcrDoubleSpinBox("adhoc_refinedDetectorOri12", new QcrCell<double>(0.0), 10, 6);
    detectorOrientation20 =
        new QcrDoubleSpinBox("adhoc_refinedDetectorOri20", new QcrCell<double>(0.0), 10, 6);
    detectorOrientation21 =
        new QcrDoubleSpinBox("adhoc_refinedDetectorOri21", new QcrCell<double>(0.0), 10, 6);
    detectorOrientation22 =
        new QcrDoubleSpinBox("adhoc_refinedDetectorOri22", new QcrCell<double>(0.0), 10, 6);
    detectorGrid->addWidget(detectorOrientation00, 0, 0, 1, 1);
    detectorGrid->addWidget(detectorOrientation01, 0, 1, 1, 1);
    detectorGrid->addWidget(detectorOrientation02, 0, 2, 1, 1);
    detectorGrid->addWidget(detectorOrientation10, 1, 0, 1, 1);
    detectorGrid->addWidget(detectorOrientation11, 1, 1, 1, 1);
    detectorGrid->addWidget(detectorOrientation12, 1, 2, 1, 1);
    detectorGrid->addWidget(detectorOrientation20, 2, 0, 1, 1);
    detectorGrid->addWidget(detectorOrientation21, 2, 1, 1, 1);
    detectorGrid->addWidget(detectorOrientation22, 2, 2, 1, 1);
    rightmiddle->addWidget(detectorOrientation);
    rightmiddle->addItem(new QSpacerItem(20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding));
    middle->addLayout(rightmiddle);
    whole->addLayout(middle);
    QHBoxLayout* below = new QHBoxLayout;
    below->addWidget(new QLabel("Frame"));
    frame = new QcrSpinBox("adhoc_refineFrame", new QcrCell<int>(0), 6);
    below->addWidget(frame);
    QSlider* slider = new QSlider(Qt::Horizontal, this);
    below->addWidget(slider);
    whole->addLayout(below);
}

void RefinerFitWidget::selectedDataChanged(int selected_data)
{
    Q_UNUSED(selected_data)

    QListWidgetItem* current_item = selectedData->currentItem();

    nsx::sptrDataSet data = current_item->data(Qt::UserRole).value<nsx::sptrDataSet>();

    nsx::Refiner refiner = _refiners.at(data);
    const std::vector<nsx::RefinementBatch>& batches = refiner.batches();

    batch->setMinimum(0);
    batch->setMaximum(batches.size() - 1);

    size_t max_frame = data->nFrames() - 1;

    frame->setMinimum(0);
    frame->setMaximum(max_frame);

    //    slider->setMinimum(0);
    //    slider->setMaximum(max_frame);

    selectedBatchChanged(0);
    selectedFrameChanged(0);
}

void RefinerFitWidget::selectedBatchChanged(int selected_batch)
{
    //    plot->clearGraphs();

    // If no data is selected, return
    QListWidgetItem* current_data_item = selectedData->currentItem();
    if (!current_data_item)
        return;

    // If no refiner is set for this data, return
    nsx::sptrDataSet data = current_data_item->data(Qt::UserRole).value<nsx::sptrDataSet>();

    // If no batches are set for this refiner, return
    nsx::Refiner refiner = _refiners.at(data);
    const std::vector<nsx::RefinementBatch>& batches = refiner.batches();

    QPen pen;
    pen.setColor(QColor("black"));
    pen.setWidth(2.0);

    //    plot->addGraph();
    //    plot->graph(0)->setPen(pen);

    // Get the cost function for this batch
    nsx::RefinementBatch batch = batches[selected_batch];
    const std::vector<double> cost_function = batch.costFunction();

    std::vector<double> iterations(cost_function.size());
    std::iota(iterations.begin(), iterations.end(), 0);

    QVector<double> x_values = QVector<double>::fromStdVector(iterations);
    QVector<double> y_values = QVector<double>::fromStdVector(cost_function);

    //    plot->graph(0)->addData(x_values, y_values);

    //    plot->xAxis->setLabel("# iterations");
    //    plot->yAxis->setLabel("Cost function");

    //    plot->setNotAntialiasedElements(QCP::aeAll);

    QFont font;
    font.setStyleStrategy(QFont::NoAntialias);
    //    plot->xAxis->setTickLabelFont(font);
    //    plot->yAxis->setTickLabelFont(font);

    //    plot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom |
    //                               QCP::iSelectAxes | QCP::iSelectLegend |
    //                               QCP::iSelectPlottables);

    //    plot->rescaleAxes();

    //    plot->replot();
}

void RefinerFitWidget::selectedFrameChanged(int selected_frame)
{
    frame->setCellValue(selected_frame);
    // slider->setValue(selected_frame);

    QListWidgetItem* current_item = selectedData->currentItem();

    // No data selected, return
    if (!current_item)
        return;

    nsx::sptrDataSet data = current_item->data(Qt::UserRole).value<nsx::sptrDataSet>();

    const nsx::InstrumentStateList& instrument_states = data->instrumentStates();

    const nsx::InstrumentState& selected_state = instrument_states[selected_frame];
    // QLabel box...
    //    QFont font;
    //    font.setBold(true);
    //    _ui->refined->setStyleSheet(selected_state.refined ? "QLabel {color : blue;}"
    //                                                       : "QLabel {color : red;}");
    //    _ui->refined->setFont(font);
    //    _ui->refined->setText(selected_state.refined ? "Refined" : "Not refined");

    // Set the sample position values
    const auto& sample_position = selected_state.samplePosition;
    samplePosX->setCellValue(sample_position[0]);
    samplePosY->setCellValue(sample_position[1]);
    samplePosZ->setCellValue(sample_position[2]);

    // Set the sample position values
    const auto& detector_position_offset = selected_state.detectorPositionOffset;
    detectorPosX->setCellValue(detector_position_offset[0]);
    detectorPosY->setCellValue(detector_position_offset[1]);
    detectorPosZ->setCellValue(detector_position_offset[2]);

    // Set the normalized incoming beam
    const auto& ni = selected_state.ni;
    niX->setCellValue(ni[0]);
    niY->setCellValue(ni[1]);
    niZ->setCellValue(ni[2]);

    const double wavelengthval = selected_state.wavelength;
    wavelength->setCellValue(wavelengthval);

    double wavelength_offset = wavelengthval * (ni.norm() - 1.0);
    wavelengthOffset->setCellValue(wavelength_offset);

    Eigen::Matrix3d sample_orientation_matrix = selected_state.sampleOrientationMatrix();
    sampleOrientation00->setCellValue(sample_orientation_matrix(0, 0));
    sampleOrientation01->setCellValue(sample_orientation_matrix(0, 1));
    sampleOrientation02->setCellValue(sample_orientation_matrix(0, 2));
    sampleOrientation10->setCellValue(sample_orientation_matrix(1, 0));
    sampleOrientation11->setCellValue(sample_orientation_matrix(1, 1));
    sampleOrientation12->setCellValue(sample_orientation_matrix(1, 2));
    sampleOrientation20->setCellValue(sample_orientation_matrix(2, 0));
    sampleOrientation21->setCellValue(sample_orientation_matrix(2, 1));
    sampleOrientation22->setCellValue(sample_orientation_matrix(2, 2));

    detectorOrientation00->setCellValue(selected_state.detectorOrientation(0, 0));
    detectorOrientation01->setCellValue(selected_state.detectorOrientation(0, 1));
    detectorOrientation02->setCellValue(selected_state.detectorOrientation(0, 2));
    detectorOrientation10->setCellValue(selected_state.detectorOrientation(1, 0));
    detectorOrientation11->setCellValue(selected_state.detectorOrientation(1, 1));
    detectorOrientation12->setCellValue(selected_state.detectorOrientation(1, 2));
    detectorOrientation20->setCellValue(selected_state.detectorOrientation(2, 0));
    detectorOrientation21->setCellValue(selected_state.detectorOrientation(2, 1));
    detectorOrientation22->setCellValue(selected_state.detectorOrientation(2, 2));
}
