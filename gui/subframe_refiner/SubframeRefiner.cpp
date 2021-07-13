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
#include "gui/MainWin.h" // gGui
#include "gui/dialogs/ListNameDialog.h"
#include "gui/frames/ProgressView.h"
#include "gui/graphics/SXPlot.h"
#include "gui/models/Meta.h"
#include "gui/models/Project.h"
#include "gui/models/Session.h"
#include "gui/subframe_refiner/RefinerTables.h"
#include "gui/utility/ColorButton.h"
#include "gui/utility/GridFiller.h"
#include "gui/utility/PropertyScrollArea.h"
#include "gui/utility/SideBar.h"
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
    auto input_box = new Spoiler("Input");
    GridFiller f(input_box, true);

    _exp_combo = f.addCombo("Experiment");
    _peak_combo = f.addCombo("Peaks");
    _data_combo = f.addCombo("Data set");
    _cell_combo = f.addCombo("Unit cell");
    _n_batches_spin = f.addSpinBox("Number of batches");

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
    auto refiner_flags_box = new Spoiler("Parameters to refine");
    GridFiller f(refiner_flags_box, true);

    _refineUB = f.addCheckBox("Cell vectors");
    _refineSamplePosition = f.addCheckBox("Sample position");
    _refineSampleOrientation = f.addCheckBox("Sample orientation");
    _refineDetectorPosition = f.addCheckBox("Detector position");
    _refineKi = f.addCheckBox("Incident wavevector");
    _refine_button = f.addButton("Refine");

    connect(_refine_button, &QPushButton::clicked, this, &SubframeRefiner::refine);
    connect(
        gGui->sideBar(), &SideBar::subframeChanged, this, &SubframeRefiner::setRefinerParameters);

    _left_layout->addWidget(refiner_flags_box);
}

void SubframeRefiner::refreshAll()
{
    updateExptList();
    toggleUnsafeWidgets();
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
    grabRefinerParameters();
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
        auto* params = expt->refinerParams();

        setRefinerParameters();

        int n_checked = 0;
        if (params->refine_ub)
            ++n_checked;
        if (params->refine_sample_position)
            ++n_checked;
        if (params->refine_sample_orientation)
            ++n_checked;
        if (params->refine_detector_offset)
            ++n_checked;
        if (params->refine_ki)
            ++n_checked;
        if (n_checked > 0)
            _refine_success = expt->refine(peaks, cell, data.get(), params->nbatches);

        auto refiner = expt->refiner();
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

    _plot_box = new Spoiler("Plot");
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
    auto update_box = new Spoiler("Update predictions");
    GridFiller f(update_box, true);

    _predicted_combo = f.addCombo("Predicted peaks");

    _update_button = f.addButton("Update", "Update peak positions given refined unit cell");

    _left_layout->addWidget(update_box);

    connect(_update_button, &QPushButton::clicked, this, &SubframeRefiner::updatePredictions);
}

void SubframeRefiner::grabRefinerParameters()
{
    auto* params =
        gSession->experimentAt(_exp_combo->currentIndex())->experiment()->refinerParams();

    _n_batches_spin->setValue(params->nbatches);
    _refineUB->setChecked(params->refine_ub);
    _refineSamplePosition->setChecked(params->refine_sample_position);
    _refineSampleOrientation->setChecked(params->refine_sample_orientation);
    _refineDetectorPosition->setChecked(params->refine_detector_offset);
    _refineKi->setChecked(params->refine_ki);
}

void SubframeRefiner::setRefinerParameters()
{
    if (_exp_combo->count() == 0)
        return;
    auto* params =
        gSession->experimentAt(_exp_combo->currentIndex())->experiment()->refinerParams();

    params->nbatches = _n_batches_spin->value();
    params->refine_ub = _refineUB->isChecked();
    params->refine_sample_position = _refineSamplePosition->isChecked();
    params->refine_sample_orientation = _refineSampleOrientation->isChecked();
    params->refine_detector_offset = _refineDetectorPosition->isChecked();
    params->refine_ki = _refineKi->isChecked();
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

void SubframeRefiner::toggleUnsafeWidgets()
{
    _refine_button->setEnabled(true);
    if (!_predicted_combo->count() == 0)
        _update_button->setEnabled(true);
    if (_exp_combo->count() == 0 || _data_combo->count() == 0 || _peak_combo->count() == 0
        || _cell_combo->count() == 0) {
        _refine_button->setEnabled(false);
        _update_button->setEnabled(false);
    }
}
