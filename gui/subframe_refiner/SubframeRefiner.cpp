//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/subframe_filter/SubframeRefiner.cpp
//! @brief     Implements class SubframeRefiner
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "gui/subframe_refiner/SubframeRefiner.h"

#include "core/experiment/Experiment.h"
#include "core/shape/PeakCollection.h"
#include "core/shape/PeakFilter.h"
#include "gui/MainWin.h"
#include "gui/dialogs/ListNameDialog.h"
#include "gui/frames/ProgressView.h"
#include "gui/graphics/SXPlot.h"
#include "gui/models/Meta.h"
#include "gui/models/Project.h"
#include "gui/models/Session.h"
#include "gui/subframe_predict/ShapeCollectionDialog.h"
#include "gui/subframe_refiner/RefinerTables.h"
#include "gui/utility/ColorButton.h"
#include "gui/utility/GridFiller.h"
#include "gui/utility/PropertyScrollArea.h"
#include "gui/utility/Spoiler.h"
#include "gui/widgets/PlotCheckBox.h"

#include <QFileInfo>
#include <QGridLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QItemDelegate>
#include <QLabel>
#include <QScrollArea>
#include <QScrollBar>
#include <QSpacerItem>
#include <sstream>

SubframeRefiner::SubframeRefiner()
{
    auto main_layout = new QHBoxLayout(this);
    auto right_element = new QSplitter(Qt::Vertical, this);

    _left_layout = new QVBoxLayout(this);

    _tables_widget = new RefinerTables();
    _tables_widget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    setInputUp();
    setRefinerFlagsUp();
    setParametersUp();
    setPlotUp();
    setUpdateUp();
    setReintegrateUp();

    right_element->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    right_element->addWidget(_tables_widget);

    _plot_widget = new SXPlot;
    _plot_widget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    right_element->addWidget(_plot_widget);

    auto propertyScrollArea = new PropertyScrollArea(this);
    propertyScrollArea->setContentLayout(_left_layout);
    main_layout->addWidget(propertyScrollArea);
    main_layout->addWidget(right_element);

    QRandomGenerator _rng(0);
}

void SubframeRefiner::setInputUp()
{
    auto input_box = new Spoiler("1. Input");
    GridFiller f(input_box, true);

    _exp_combo = f.addCombo("Experiment");
    _peak_combo = f.addCombo("Peaks");
    _data_combo = f.addCombo("Data set");
    _cell_combo = f.addCombo("Unit cell");
    _n_batches_spin = f.addSpinBox("Number of batches");

    _n_batches_spin->setValue(_refiner_params.nbatches);
    _n_batches_spin->setMinimum(1);
    _n_batches_spin->setMaximum(1000); // updated on setBatchesUp

    connect(
        _exp_combo, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this,
        &SubframeRefiner::updateDatasetList);
    connect(
        _exp_combo, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this,
        &SubframeRefiner::updateUnitCellList);
    connect(
        _exp_combo, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this,
        &SubframeRefiner::updatePeakList);

    _left_layout->addWidget(input_box);
}

void SubframeRefiner::setRefinerFlagsUp()
{
    auto refiner_flags_box = new Spoiler("2. Parameters to refine");
    GridFiller f(refiner_flags_box, true);

    _refineUB = f.addCheckBox("Cell vectors");
    _refineSamplePosition = f.addCheckBox("Sample position");
    _refineSampleOrientation = f.addCheckBox("Sample orientation");
    _refineDetectorPosition = f.addCheckBox("Detector position");
    _refineKi = f.addCheckBox("Incident wavevector");
    auto refine_button = f.addButton("Refine");

    _refineUB->setChecked(_refiner_params.refine_ub);
    _refineSamplePosition->setChecked(_refiner_params.refine_sample_position);
    _refineSampleOrientation->setChecked(_refiner_params.refine_sample_orientation);
    _refineDetectorPosition->setChecked(_refiner_params.refine_detector_offset);
    _refineKi->setChecked(_refiner_params.refine_ki);

    connect(refine_button, &QPushButton::clicked, this, &SubframeRefiner::refine);

    _left_layout->addWidget(refiner_flags_box);
}

void SubframeRefiner::refreshAll()
{
    updateExptList();
    // refreshTables();
}

void SubframeRefiner::refreshTables()
{
    const auto expt = gSession->experimentAt(_exp_combo->currentIndex())->experiment();
    const auto data = expt->getData(_data_combo->currentText().toStdString());
    const auto refiner = expt->refiner();
    _tables_widget->refreshTables(refiner, data.get());
}

void SubframeRefiner::setParametersUp()
{
    updateExptList();
}

void SubframeRefiner::updateExptList()
{
    _exp_combo->blockSignals(true);
    QString current_exp = _exp_combo->currentText();
    _exp_combo->clear();

    if (gSession->experimentNames().empty())
        return;

    for (const QString& exp : gSession->experimentNames())
        _exp_combo->addItem(exp);
    _exp_combo->setCurrentText(current_exp);

    _exp_combo->blockSignals(false);
    updateDatasetList();
    updatePeakList();
    updateUnitCellList();
    _refiner_params =
        gSession->experimentAt(_exp_combo->currentIndex())->experiment()->refiner_params;
    _shape_params = gSession->experimentAt(_exp_combo->currentIndex())->experiment()->shape_params;
}

void SubframeRefiner::updateDatasetList()
{
    _data_combo->blockSignals(true);

    QString current_data = _data_combo->currentText();
    _data_combo->clear();
    _data_list = gSession->experimentAt(_exp_combo->currentIndex())->allData();

    const QStringList& datanames{gSession->currentProject()->getDataNames()};
    if (!datanames.empty()) {
        _data_combo->addItems(datanames);
        _data_combo->setCurrentText(current_data);
    }
    _data_combo->blockSignals(false);
    setBatchesUp();
}

void SubframeRefiner::updatePeakList()
{
    _peak_combo->blockSignals(true);
    QString current_peaks = _peak_combo->currentText();
    _peak_combo->clear();
    _peak_list.clear();

    QStringList tmp = gSession->experimentAt(_exp_combo->currentIndex())
                          ->getPeakCollectionNames(nsx::listtype::FOUND);
    _peak_list.append(tmp);
    tmp.clear();
    tmp = gSession->experimentAt(_exp_combo->currentIndex())
              ->getPeakCollectionNames(nsx::listtype::PREDICTED);
    _peak_list.append(tmp);
    tmp.clear();
    tmp = gSession->experimentAt(_exp_combo->currentIndex())
              ->getPeakCollectionNames(nsx::listtype::FILTERED);
    _peak_list.append(tmp);

    if (!_peak_list.empty()) {
        _peak_combo->addItems(_peak_list);
        _peak_combo->setCurrentText(current_peaks);
    }

    _peak_combo->blockSignals(false);
    updatePredictedList();
    refreshPeakShapeStatus();
}

void SubframeRefiner::updateUnitCellList()
{
    _cell_combo->blockSignals(true);
    QString current_cell = _cell_combo->currentText();
    _cell_combo->clear();

    _cell_list = gSession->experimentAt(_exp_combo->currentIndex())->getUnitCellNames();

    if (!_cell_list.empty()) {
        _cell_combo->addItems(_cell_list);
        _cell_combo->setCurrentText(current_cell);
    }
    _cell_combo->blockSignals(false);
}

void SubframeRefiner::setBatchesUp()
{
    const auto dataset =
        gSession->experimentAt(_exp_combo->currentIndex())->getData(_data_combo->currentIndex());
    if (dataset)
        _n_batches_spin->setMaximum(dataset->nFrames());
}

void SubframeRefiner::refine()
{
    try {
        auto expt = gSession->experimentAt(_exp_combo->currentIndex())->experiment();
        const auto data = expt->getData(_data_combo->currentText().toStdString());
        const auto peaks = expt->getPeakCollection(_peak_combo->currentText().toStdString());
        const auto cell = expt->getUnitCell(_cell_combo->currentText().toStdString());
        const auto peak_list = peaks->getPeakList();
        auto states = data->instrumentStates();

        nsx::RefinerParameters params{};
        params.nbatches = _n_batches_spin->value();
        params.refine_ub = false;
        params.refine_sample_position = false;
        params.refine_detector_offset = false;
        params.refine_sample_orientation = false;
        params.refine_ki = false;

        int n_checked = 0;
        if (_refineUB->isChecked()) {
            params.refine_ub = true;
            ++n_checked;
        }
        if (_refineSamplePosition->isChecked()) {
            params.refine_sample_position = true;
            ++n_checked;
        }
        if (_refineSampleOrientation->isChecked()) {
            params.refine_sample_orientation = true;
            ++n_checked;
        }
        if (_refineDetectorPosition->isChecked()) {
            params.refine_detector_offset = true;
            ++n_checked;
        }
        if (_refineKi->isChecked()) {
            params.refine_ki = true;
            ++n_checked;
        }

        if (n_checked > 0) {
            _refine_success = expt->refine(peaks, cell, data.get(), params);
        }

        const auto refiner = expt->refiner();
        _tables_widget->refreshTables(refiner, data.get());
    } catch (const std::exception& ex) {
        QMessageBox::critical(this, "Error", QString(ex.what()));
    }
}

void SubframeRefiner::setPlotUp()
{
    // Shortener for creating PlotCheckBox
    const auto cb = [](const QString& text, TableType table, int column) {
        return new PlotCheckBox(text, table, column);
    };

    _plot_box = new Spoiler("3. Plot");
    QVBoxLayout* plot_layout = new QVBoxLayout();

    // Create group & gridlayout for checkboxes. Add it already to the main layout
    // and return its grid layout
    const auto addGroup = [&](const QString& title) -> QGridLayout* {
        QGroupBox* group = new QGroupBox(title);
        QGridLayout* grid = new QGridLayout(group);
        group->setLayout(grid);
        plot_layout->addWidget(group);
        return grid;
    };

    // -- Lattice checks group
    auto lattice_grid = addGroup("Cell parameters");
    lattice_grid->addWidget(cb("a", TableType::Lattice, 1), 0, 0);
    lattice_grid->addWidget(cb("b", TableType::Lattice, 2), 0, 1);
    lattice_grid->addWidget(cb("c", TableType::Lattice, 3), 0, 2);
    lattice_grid->addWidget(cb(QString((QChar)0x03B1), TableType::Lattice, 4), 1, 0);
    lattice_grid->addWidget(cb(QString((QChar)0x03B2), TableType::Lattice, 5), 1, 1);
    lattice_grid->addWidget(cb(QString((QChar)0x03B3), TableType::Lattice, 6), 1, 2);

    // -- Sample position checks group
    auto sample_pos_grid = addGroup("Sample position");
    sample_pos_grid->addWidget(cb("x", TableType::SamplePos, 1), 0, 0);
    sample_pos_grid->addWidget(cb("y", TableType::SamplePos, 2), 0, 1);
    sample_pos_grid->addWidget(cb("z", TableType::SamplePos, 3), 0, 2);

    // -- Sample orientation checks group
    auto sample_orn_grid = addGroup("Sample orientation");
    sample_orn_grid->addWidget(cb("xx", TableType::SampleOrn, 1), 0, 0);
    sample_orn_grid->addWidget(cb("xy", TableType::SampleOrn, 2), 0, 1);
    sample_orn_grid->addWidget(cb("xz", TableType::SampleOrn, 3), 0, 2);
    sample_orn_grid->addWidget(cb("yx", TableType::SampleOrn, 4), 1, 0);
    sample_orn_grid->addWidget(cb("yy", TableType::SampleOrn, 5), 1, 1);
    sample_orn_grid->addWidget(cb("yz", TableType::SampleOrn, 6), 1, 2);
    sample_orn_grid->addWidget(cb("zx", TableType::SampleOrn, 7), 2, 0);
    sample_orn_grid->addWidget(cb("zy", TableType::SampleOrn, 8), 2, 1);
    sample_orn_grid->addWidget(cb("zz", TableType::SampleOrn, 9), 2, 2);

    // - Detector offset checks group
    auto detector_pos_grid = addGroup("Detector offset");
    detector_pos_grid->addWidget(cb("x", TableType::DetectorPos, 1), 0, 0);
    detector_pos_grid->addWidget(cb("y", TableType::DetectorPos, 2), 0, 1);
    detector_pos_grid->addWidget(cb("z", TableType::DetectorPos, 3), 0, 2);

    // -- Incident wavevector checks group
    auto ki_grid = addGroup("Incident wavevector");
    ki_grid->addWidget(cb("x", TableType::Ki, 1), 0, 0);
    ki_grid->addWidget(cb("y", TableType::Ki, 2), 0, 1);
    ki_grid->addWidget(cb("z", TableType::Ki, 3), 0, 2);

    _plot_box->setContentLayout(*plot_layout, true);

    _left_layout->addWidget(_plot_box);

    // refresh the plot whenever a PlotCheckBox state changes
    for (auto checkbox : plotCheckBoxes())
        connect(checkbox, &QCheckBox::stateChanged, this, &SubframeRefiner::refreshPlot);
}

void SubframeRefiner::refreshPlot()
{
    _plot_widget->clearGraphs();
    _plot_widget->xAxis->setLabel("frame");
    _plot_widget->yAxis->setLabel(QString(QChar(0x0394)));
    _plot_widget->setNotAntialiasedElements(QCP::aeAll);

    QPen pen;
    pen.setWidth(2.0);

    QFont font;
    font.setStyleStrategy(QFont::NoAntialias);
    _plot_widget->xAxis->setTickLabelFont(font);
    _plot_widget->yAxis->setTickLabelFont(font);
    _plot_widget->setInteractions(
        QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectAxes | QCP::iSelectLegend
        | QCP::iSelectPlottables);

    for (PlotCheckBox* check : plotCheckBoxes()) {
        if (check->isChecked()) {
            QVector<double> xvals = _tables_widget->getXVals();
            QVector<double> yvals = _tables_widget->getYVals(check->getTable(), check->getColumn());

            QColor color(_rng.bounded(256), _rng.bounded(256), _rng.bounded(256));
            pen.setColor(color);

            _plot_widget->addGraph();
            _plot_widget->graph()->setPen(pen);
            _plot_widget->graph()->addData(xvals, yvals);
            _plot_widget->graph()->setName(check->getLabel());
        }
    }
    _plot_widget->rescaleAxes();
    _plot_widget->replot();
}

void SubframeRefiner::setUpdateUp()
{
    auto update_box = new Spoiler("4. Update predictions");
    GridFiller f(update_box, true);

    _predicted_combo = f.addCombo("Predicted peaks");

    auto update_button = f.addButton("Update", "Update peak positions given refined unit cell");

    auto build_shape_lib = f.addButton(
        "Build shape collection",
        "<font>A shape collection is a collection of averaged peaks attached to a peak"
        "collection. A shape is the averaged peak shape of a peak and its neighbours within a "
        "specified cutoff.</font>"); // Rich text to force line break in tooltip

    _left_layout->addWidget(update_box);

    connect(update_button, &QPushButton::clicked, this, &SubframeRefiner::updatePredictions);
    connect(build_shape_lib, &QPushButton::clicked, this, &SubframeRefiner::openShapeBuilder);
}

void SubframeRefiner::updatePredictedList()
{
    _predicted_combo->blockSignals(true);
    _predicted_combo->clear();

    _predicted_list = gSession->experimentAt(_exp_combo->currentIndex())
                          ->getPeakCollectionNames(nsx::listtype::PREDICTED);

    if (!_predicted_list.empty()) {
        _predicted_combo->addItems(_predicted_list);
        _predicted_combo->setCurrentIndex(0);
    }
    _predicted_combo->blockSignals(false);
}

void SubframeRefiner::updatePredictions()
{
    if (_refine_success) {
        auto expt = gSession->experimentAt(_exp_combo->currentIndex())->experiment();
        auto peaks = expt->getPeakCollection(_predicted_combo->currentText().toStdString());
        expt->updatePredictions(peaks);
    } else {
        QMessageBox::critical(this, "Error", "Cannot update predictions: refinement failed");
    }
}

QList<PlotCheckBox*> SubframeRefiner::plotCheckBoxes() const
{
    return _plot_box->findChildren<PlotCheckBox*>();
}

void SubframeRefiner::setReintegrateUp()
{
    _reintegrate_box = new Spoiler("5. Reintegrate peaks");
    GridFiller f(_reintegrate_box, true);

    // -- Create controls
    _integrator_combo = f.addCombo();

    _fit_center =
        f.addCheckBox("Fit the center", "Allow the peak center to move during integration");

    _fit_covariance = f.addCheckBox(
        "Fit the covariance", "Allow the peak covariance matrix to vary during integration");

    _peak_end_int = f.addDoubleSpinBox("Peak end", "(sigmas) - scaling factor for peak region");

    _bkg_start_int =
        f.addDoubleSpinBox("Bkg begin:", "(sigmas) - scaling factor for lower limit of background");

    _bkg_end_int =
        f.addDoubleSpinBox("Bkg end:", "(sigmas) - scaling factor for upper limit of background");

    _radius_int =
        f.addDoubleSpinBox("Search radius:", "(pixels) - neighbour search radius in pixels");

    _n_frames_int =
        f.addDoubleSpinBox("N. of frames:", "(frames) - neighbour search radius in frames");

    auto reintegrate_found = f.addButton("Reintegrate found peaks");
    auto reintegrate_predicted = f.addButton("Reintegrate predicted peaks");

    // -- Initialize controls
    _integrator_combo->addItem("Pixel sum integrator");
    _integrator_combo->addItem("Gaussian integrator");
    _integrator_combo->addItem("I/Sigma integrator");
    _integrator_combo->addItem("1d profile integrator");
    _integrator_combo->addItem("3d profile integrator");

    _fit_center->setChecked(_refiner_params.fit_center);

    _fit_covariance->setChecked(_refiner_params.fit_cov);

    _peak_end_int->setMaximum(100000);
    _peak_end_int->setDecimals(2);
    _peak_end_int->setValue(_refiner_params.peak_end);

    _bkg_start_int->setMaximum(100000);
    _bkg_start_int->setDecimals(2);
    _bkg_start_int->setValue(_refiner_params.bkg_begin);

    _bkg_end_int->setMaximum(100000);
    _bkg_end_int->setDecimals(2);
    _bkg_end_int->setValue(_refiner_params.bkg_end);

    _radius_int->setMaximum(100000);
    _radius_int->setDecimals(2);
    _radius_int->setValue(_refiner_params.neighbour_range_pixels);

    _n_frames_int->setMaximum(100000);
    _n_frames_int->setDecimals(2);
    _n_frames_int->setValue(_refiner_params.neighbour_range_frames);

    connect(reintegrate_found, &QPushButton::clicked, this, &SubframeRefiner::reintegrateFound);
    connect(
        reintegrate_predicted, &QPushButton::clicked, this, &SubframeRefiner::reintegratePredicted);

    _left_layout->addWidget(_reintegrate_box);
}

void SubframeRefiner::runReintegration(nsx::PeakCollection* peaks)
{
    try {
        nsx::sptrProgressHandler handler(new nsx::ProgressHandler);
        ProgressView progressView(nullptr);
        progressView.watch(handler);

        nsx::Experiment* expt = gSession->experimentAt(_exp_combo->currentIndex())->experiment();
        nsx::IPeakIntegrator* integrator =
            expt->getIntegrator(_integrator_combo->currentText().toStdString());
        nsx::PeakCollection* found_peaks =
            expt->getPeakCollection(_peak_combo->currentText().toStdString());
        nsx::ShapeCollection* shapes = found_peaks->shapeCollection();

        nsx::IntegrationParameters params;
        params.peak_end = _peak_end_int->value();
        params.bkg_begin = _bkg_start_int->value();
        params.bkg_end = _bkg_end_int->value();
        params.neighbour_range_pixels = _radius_int->value();
        params.neighbour_range_frames = _n_frames_int->value();
        params.fit_center = _fit_center->isChecked();
        params.fit_cov = _fit_covariance->isChecked();

        integrator->setHandler(handler);
        expt->integratePeaks(integrator, peaks, &params, shapes);
    } catch (std::exception& e) {
        QMessageBox::critical(this, "Error", QString(e.what()));
    }
}

void SubframeRefiner::reintegrateFound()
{
    // #nsxAudit Crash if no experiment existing. Disable btn if no experiment loaded?
    nsx::Experiment* expt = gSession->experimentAt(_exp_combo->currentIndex())->experiment();
    nsx::PeakCollection* found_peaks =
        expt->getPeakCollection(_peak_combo->currentText().toStdString());
    runReintegration(found_peaks);
}

void SubframeRefiner::reintegratePredicted()
{
    // #nsxAudit Crash if no experiment existing. Disable btn if no experiment loaded?
    nsx::Experiment* expt = gSession->experimentAt(_exp_combo->currentIndex())->experiment();
    nsx::PeakCollection* predicted_peaks =
        expt->getPeakCollection(_predicted_combo->currentText().toStdString());
    runReintegration(predicted_peaks);
}

void SubframeRefiner::openShapeBuilder()
{
    // #nsxAudit Crash if no experiment existing. Disable btn if no experiment loaded?
    nsx::PeakCollection* peak_collection =
        gSession->experimentAt(_exp_combo->currentIndex())
            ->experiment()
            ->getPeakCollection(_peak_combo->currentText().toStdString());

    std::unique_ptr<ShapeCollectionDialog> dialog(
        new ShapeCollectionDialog(peak_collection, _shape_params));

    dialog->exec();
    refreshPeakShapeStatus();
}


void SubframeRefiner::refreshPeakShapeStatus()
{
    bool shape_collection_present = true;

    if (_peak_list.empty() || _exp_combo->count() < 1)
        shape_collection_present = false;

    if (shape_collection_present) {
        nsx::PeakCollection* collection =
            gSession->experimentAt(_exp_combo->currentIndex())
                ->experiment()
                ->getPeakCollection(_peak_combo->currentText().toStdString());
        if (collection->shapeCollection() == nullptr)
            shape_collection_present = false;
    }

    _reintegrate_box->setEnabled(shape_collection_present);
}
