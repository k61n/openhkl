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

SubframeRefiner::SubframeRefiner() : QWidget()
{
    setSizePolicies();
    _main_layout = new QHBoxLayout(this);
    _right_element = new QSplitter(Qt::Vertical, this);

    QScrollArea* scroll_area = new QScrollArea(this);
    QWidget* scroll_widget = new QWidget();
    scroll_area->setSizePolicy(*_size_policy_box);
    scroll_widget->setSizePolicy(*_size_policy_box);
    _left_layout = new QVBoxLayout(scroll_widget);
    scroll_area->setWidgetResizable(true);
    scroll_area->setWidget(scroll_widget);

    _tables_widget = new RefinerTables();
    _tables_widget->setSizePolicy(*_size_policy_right);

    setInputUp();
    setRefinerFlagsUp();
    setParametersUp();
    setPlotUp();
    setUpdateUp();
    setReintegrateUp();

    _left_layout->addItem(new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding));

    _right_element->setSizePolicy(*_size_policy_right);
    _right_element->addWidget(_tables_widget);

    _plot_widget = new SXPlot;
    _plot_widget->setSizePolicy(*_size_policy_right);
    _right_element->addWidget(_plot_widget);

    _main_layout->addWidget(scroll_area);
    _main_layout->addWidget(_right_element);

    QRandomGenerator _rng(0);
}

SubframeRefiner::~SubframeRefiner()
{
    delete _size_policy_widgets;
    delete _size_policy_box;
    delete _size_policy_right;
    delete _size_policy_fixed;
}

void SubframeRefiner::setSizePolicies()
{
    _size_policy_widgets = new QSizePolicy();
    _size_policy_widgets->setHorizontalPolicy(QSizePolicy::Preferred);
    _size_policy_widgets->setVerticalPolicy(QSizePolicy::Fixed);

    _size_policy_box = new QSizePolicy();
    _size_policy_box->setHorizontalPolicy(QSizePolicy::Preferred);
    _size_policy_box->setVerticalPolicy(QSizePolicy::Expanding);

    _size_policy_right = new QSizePolicy();
    _size_policy_right->setHorizontalPolicy(QSizePolicy::Expanding);
    _size_policy_right->setVerticalPolicy(QSizePolicy::Expanding);

    _size_policy_fixed = new QSizePolicy();
    _size_policy_fixed->setHorizontalPolicy(QSizePolicy::Fixed);
    _size_policy_fixed->setVerticalPolicy(QSizePolicy::Fixed);
}

void SubframeRefiner::setInputUp()
{
    _input_box = new Spoiler("1. Input");

    QGridLayout* _input_grid = new QGridLayout();

    QLabel* exp_label = new QLabel("Experiment");
    exp_label->setAlignment(Qt::AlignRight);
    _input_grid->addWidget(exp_label, 0, 0, 1, 1);

    QLabel* peak_label = new QLabel("Peaks");
    peak_label->setAlignment(Qt::AlignRight);
    _input_grid->addWidget(peak_label, 1, 0, 1, 1);

    QLabel* data_label = new QLabel("Data set");
    data_label->setAlignment(Qt::AlignRight);
    _input_grid->addWidget(data_label, 2, 0, 1, 1);

    QLabel* cell_label = new QLabel("Unit cell");
    cell_label->setAlignment(Qt::AlignRight);
    _input_grid->addWidget(cell_label, 3, 0, 1, 1);

    QLabel* nbatches_label = new QLabel("Number of batches");
    nbatches_label->setAlignment(Qt::AlignRight);
    _input_grid->addWidget(nbatches_label, 4, 0, 1, 1);

    _exp_combo = new QComboBox();
    _peak_combo = new QComboBox();
    _data_combo = new QComboBox();
    _cell_combo = new QComboBox();
    _n_batches_spin = new QSpinBox();

    _n_batches_spin->setValue(_refiner_params.nbatches);
    _n_batches_spin->setMinimum(1);
    _n_batches_spin->setMaximum(1000); // updated on setBatchesUp

    _exp_combo->setMaximumWidth(1000);
    _data_combo->setMaximumWidth(1000);
    _peak_combo->setMaximumWidth(1000);
    _cell_combo->setMaximumWidth(1000);
    _n_batches_spin->setMaximum(1000);

    _exp_combo->setSizePolicy(*_size_policy_widgets);
    _peak_combo->setSizePolicy(*_size_policy_widgets);
    _data_combo->setSizePolicy(*_size_policy_widgets);
    _cell_combo->setSizePolicy(*_size_policy_widgets);
    _n_batches_spin->setSizePolicy(*_size_policy_widgets);

    _input_grid->addWidget(_exp_combo, 0, 1, 1, 1);
    _input_grid->addWidget(_peak_combo, 1, 1, 1, 1);
    _input_grid->addWidget(_data_combo, 2, 1, 1, 1);
    _input_grid->addWidget(_cell_combo, 3, 1, 1, 1);
    _input_grid->addWidget(_n_batches_spin, 4, 1, 1, 1);

    connect(
        _exp_combo, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this,
        &SubframeRefiner::updateDatasetList);
    connect(
        _exp_combo, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this,
        &SubframeRefiner::updateUnitCellList);
    connect(
        _exp_combo, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this,
        &SubframeRefiner::updatePeakList);

    _input_box->setContentLayout(*_input_grid, true);
    _input_box->setSizePolicy(*_size_policy_box);
    _input_box->contentArea.setSizePolicy(*_size_policy_box);

    _left_layout->addWidget(_input_box);
    _input_box->toggler(true);
}

void SubframeRefiner::setRefinerFlagsUp()
{
    _refiner_flags_box = new Spoiler("2. Parameters to refine");

    QGridLayout* refiner_flags_layout = new QGridLayout();
    _refineUB = new QCheckBox("Cell vectors");
    _refineSamplePosition = new QCheckBox("Sample position");
    _refineSampleOrientation = new QCheckBox("Sample orientation");
    _refineDetectorPosition = new QCheckBox("Detector position");
    _refineKi = new QCheckBox("Incident wavevector");

    _refineUB->setMaximumWidth(1000);
    _refineSamplePosition->setMaximumWidth(1000);
    _refineSampleOrientation->setMaximumWidth(1000);
    _refineDetectorPosition->setMaximumWidth(1000);
    _refineKi->setMaximumWidth(1000);

    _refineUB->setSizePolicy(*_size_policy_widgets);
    _refineSamplePosition->setSizePolicy(*_size_policy_widgets);
    _refineSampleOrientation->setSizePolicy(*_size_policy_widgets);
    _refineDetectorPosition->setSizePolicy(*_size_policy_widgets);
    _refineKi->setSizePolicy(*_size_policy_widgets);

    _refine_button = new QPushButton("Refine");
    _refine_button->setSizePolicy(*_size_policy_widgets);

    refiner_flags_layout->addWidget(_refineUB, 0, 0, 1, 2);
    refiner_flags_layout->addWidget(_refineSamplePosition, 1, 0, 1, 2);
    refiner_flags_layout->addWidget(_refineSampleOrientation, 2, 0, 1, 2);
    refiner_flags_layout->addWidget(_refineDetectorPosition, 3, 0, 1, 2);
    refiner_flags_layout->addWidget(_refineKi, 4, 0, 1, 2);
    refiner_flags_layout->addWidget(_refine_button, 5, 0, 1, 2);

    _refiner_flags_box->setContentLayout(*refiner_flags_layout);
    _refiner_flags_box->setSizePolicy(*_size_policy_box);
    _refiner_flags_box->contentArea.setSizePolicy(*_size_policy_box);

    _refineUB->setChecked(_refiner_params.refine_ub);
    _refineSamplePosition->setChecked(_refiner_params.refine_sample_position);
    _refineSampleOrientation->setChecked(_refiner_params.refine_sample_orientation);
    _refineDetectorPosition->setChecked(_refiner_params.refine_detector_offset);
    _refineKi->setChecked(_refiner_params.refine_ki);

    connect(_refine_button, &QPushButton::clicked, this, &SubframeRefiner::refine);

    _left_layout->addWidget(_refiner_flags_box);
    _refiner_flags_box->toggler(true);
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
    _exp_combo->clear();

    if (gSession->experimentNames().empty())
        return;

    for (const QString& exp : gSession->experimentNames())
        _exp_combo->addItem(exp);

    _exp_combo->blockSignals(false);
    updateDatasetList();
    updatePeakList();
    updateUnitCellList();
    _refiner_params = gSession->experimentAt(_exp_combo->currentIndex())->experiment()->refiner_params;
    _shape_params = gSession->experimentAt(_exp_combo->currentIndex())->experiment()->shape_params;
}

void SubframeRefiner::updateDatasetList()
{
    _data_combo->blockSignals(true);
    _data_combo->clear();

    _data_list = gSession->experimentAt(_exp_combo->currentIndex())->allData();

    if (!_data_list.empty()) {
        for (const nsx::sptrDataSet& data : _data_list) {
            QFileInfo fileinfo(QString::fromStdString(data->filename()));
            _data_combo->addItem(fileinfo.baseName() /*absoluteFilePath()*/);
        }
        _data_combo->setCurrentIndex(0);
    }
    _data_combo->blockSignals(false);
    setBatchesUp();
}

void SubframeRefiner::updatePeakList()
{
    _peak_combo->blockSignals(true);
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
        _peak_combo->setCurrentIndex(0);
    }

    _peak_combo->blockSignals(false);
    updatePredictedList();
    refreshPeakShapeStatus();
}

void SubframeRefiner::updateUnitCellList()
{
    _cell_combo->blockSignals(true);
    _cell_combo->clear();

    _cell_list = gSession->experimentAt(_exp_combo->currentIndex())->getUnitCellNames();

    if (!_cell_list.empty()) {
        _cell_combo->addItems(_cell_list);
        _cell_combo->setCurrentIndex(0);
    }
    _cell_combo->blockSignals(false);
}

void SubframeRefiner::setBatchesUp()
{
    const auto dataset =
        gSession->experimentAt(_exp_combo->currentIndex())->getData(_data_combo->currentIndex());
    _n_batches_spin->setMaximum(dataset->nFrames());
}

void SubframeRefiner::refine()
{
    try {
        auto expt = gSession->experimentAt(_exp_combo->currentIndex())->experiment();
        const auto data = expt->dataShortName(_data_combo->currentText().toStdString());
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
    _plot_box = new Spoiler("3. Plot");
    QVBoxLayout* plot_layout = new QVBoxLayout();
    PlotCheckBox* check_ptr;

    QGroupBox* lattice_checks = new QGroupBox("Cell parameters");
    QGridLayout* lattice_grid = new QGridLayout();

    check_ptr = new PlotCheckBox("a", TableType::Lattice, 1);
    check_ptr->setMaximumWidth(1000);
    check_ptr->setSizePolicy(*_size_policy_widgets);
    lattice_grid->addWidget(check_ptr, 0, 0, 1, 1);
    _plot_check_boxes.push_back(check_ptr);
    check_ptr = new PlotCheckBox("b", TableType::Lattice, 2);
    check_ptr->setMaximumWidth(1000);
    check_ptr->setSizePolicy(*_size_policy_widgets);
    lattice_grid->addWidget(check_ptr, 0, 1, 1, 1);
    _plot_check_boxes.push_back(check_ptr);
    check_ptr = new PlotCheckBox("c", TableType::Lattice, 3);
    check_ptr->setMaximumWidth(1000);
    check_ptr->setSizePolicy(*_size_policy_widgets);
    lattice_grid->addWidget(check_ptr, 0, 2, 1, 1);
    _plot_check_boxes.push_back(check_ptr);
    check_ptr = new PlotCheckBox(QString((QChar)0x03B1), TableType::Lattice, 4);
    check_ptr->setMaximumWidth(1000);
    check_ptr->setSizePolicy(*_size_policy_widgets);
    lattice_grid->addWidget(check_ptr, 1, 0, 1, 1);
    _plot_check_boxes.push_back(check_ptr);
    check_ptr = new PlotCheckBox(QString((QChar)0x03B2), TableType::Lattice, 5);
    check_ptr->setMaximumWidth(1000);
    check_ptr->setSizePolicy(*_size_policy_widgets);
    lattice_grid->addWidget(check_ptr, 1, 1, 1, 1);
    _plot_check_boxes.push_back(check_ptr);
    check_ptr = new PlotCheckBox(QString((QChar)0x03B3), TableType::Lattice, 6);
    check_ptr->setMaximumWidth(1000);
    check_ptr->setSizePolicy(*_size_policy_widgets);
    lattice_grid->addWidget(check_ptr, 1, 2, 1, 1);
    _plot_check_boxes.push_back(check_ptr);

    lattice_checks->setLayout(lattice_grid);
    lattice_checks->setSizePolicy(*_size_policy_box);

    QGroupBox* sample_pos_checks = new QGroupBox("Sample position");
    QGridLayout* sample_pos_grid = new QGridLayout();

    check_ptr = new PlotCheckBox("x", TableType::SamplePos, 1);
    check_ptr->setMaximumWidth(1000);
    check_ptr->setSizePolicy(*_size_policy_widgets);
    sample_pos_grid->addWidget(check_ptr, 0, 0, 1, 1);
    _plot_check_boxes.push_back(check_ptr);
    check_ptr = new PlotCheckBox("y", TableType::SamplePos, 2);
    check_ptr->setMaximumWidth(1000);
    check_ptr->setSizePolicy(*_size_policy_widgets);
    sample_pos_grid->addWidget(check_ptr, 0, 1, 1, 1);
    _plot_check_boxes.push_back(check_ptr);
    check_ptr = new PlotCheckBox("z", TableType::SamplePos, 3);
    check_ptr->setMaximumWidth(1000);
    check_ptr->setSizePolicy(*_size_policy_widgets);
    sample_pos_grid->addWidget(check_ptr, 0, 2, 1, 1);
    _plot_check_boxes.push_back(check_ptr);

    sample_pos_checks->setLayout(sample_pos_grid);
    sample_pos_checks->setSizePolicy(*_size_policy_box);

    QGroupBox* sample_orn_checks = new QGroupBox("Sample orientation");
    QGridLayout* sample_orn_grid = new QGridLayout();

    check_ptr = new PlotCheckBox("xx", TableType::SampleOrn, 1);
    check_ptr->setMaximumWidth(1000);
    check_ptr->setSizePolicy(*_size_policy_widgets);
    sample_orn_grid->addWidget(check_ptr, 0, 0, 1, 1);
    _plot_check_boxes.push_back(check_ptr);
    check_ptr = new PlotCheckBox("xy", TableType::SampleOrn, 2);
    check_ptr->setMaximumWidth(1000);
    check_ptr->setSizePolicy(*_size_policy_widgets);
    sample_orn_grid->addWidget(check_ptr, 0, 1, 1, 1);
    _plot_check_boxes.push_back(check_ptr);
    check_ptr = new PlotCheckBox("xz", TableType::SampleOrn, 3);
    check_ptr->setMaximumWidth(1000);
    check_ptr->setSizePolicy(*_size_policy_widgets);
    sample_orn_grid->addWidget(check_ptr, 0, 2, 1, 1);
    _plot_check_boxes.push_back(check_ptr);
    check_ptr = new PlotCheckBox("yx", TableType::SampleOrn, 4);
    check_ptr->setMaximumWidth(1000);
    check_ptr->setSizePolicy(*_size_policy_widgets);
    sample_orn_grid->addWidget(check_ptr, 1, 0, 1, 1);
    _plot_check_boxes.push_back(check_ptr);
    check_ptr = new PlotCheckBox("yy", TableType::SampleOrn, 5);
    check_ptr->setMaximumWidth(1000);
    check_ptr->setSizePolicy(*_size_policy_widgets);
    sample_orn_grid->addWidget(check_ptr, 1, 1, 1, 1);
    _plot_check_boxes.push_back(check_ptr);
    check_ptr = new PlotCheckBox("yz", TableType::SampleOrn, 6);
    check_ptr->setMaximumWidth(1000);
    check_ptr->setSizePolicy(*_size_policy_widgets);
    sample_orn_grid->addWidget(check_ptr, 1, 2, 1, 1);
    _plot_check_boxes.push_back(check_ptr);
    check_ptr = new PlotCheckBox("zx", TableType::SampleOrn, 7);
    check_ptr->setMaximumWidth(1000);
    check_ptr->setSizePolicy(*_size_policy_widgets);
    sample_orn_grid->addWidget(check_ptr, 2, 0, 1, 1);
    _plot_check_boxes.push_back(check_ptr);
    check_ptr = new PlotCheckBox("zy", TableType::SampleOrn, 8);
    check_ptr->setMaximumWidth(1000);
    check_ptr->setSizePolicy(*_size_policy_widgets);
    sample_orn_grid->addWidget(check_ptr, 2, 1, 1, 1);
    _plot_check_boxes.push_back(check_ptr);
    check_ptr = new PlotCheckBox("zz", TableType::SampleOrn, 9);
    check_ptr->setMaximumWidth(1000);
    check_ptr->setSizePolicy(*_size_policy_widgets);
    sample_orn_grid->addWidget(check_ptr, 2, 2, 1, 1);
    _plot_check_boxes.push_back(check_ptr);

    sample_orn_checks->setLayout(sample_orn_grid);
    sample_orn_checks->setSizePolicy(*_size_policy_box);

    QGroupBox* detector_pos_checks = new QGroupBox("Detector offset");
    QGridLayout* detector_pos_grid = new QGridLayout();

    check_ptr = new PlotCheckBox("x", TableType::DetectorPos, 1);
    check_ptr->setMaximumWidth(1000);
    check_ptr->setSizePolicy(*_size_policy_widgets);
    detector_pos_grid->addWidget(check_ptr, 0, 0, 1, 1);
    _plot_check_boxes.push_back(check_ptr);
    check_ptr = new PlotCheckBox("y", TableType::DetectorPos, 2);
    check_ptr->setMaximumWidth(1000);
    check_ptr->setSizePolicy(*_size_policy_widgets);
    detector_pos_grid->addWidget(check_ptr, 0, 1, 1, 1);
    _plot_check_boxes.push_back(check_ptr);
    check_ptr = new PlotCheckBox("z", TableType::DetectorPos, 3);
    check_ptr->setMaximumWidth(1000);
    check_ptr->setSizePolicy(*_size_policy_widgets);
    detector_pos_grid->addWidget(check_ptr, 0, 2, 1, 1);
    _plot_check_boxes.push_back(check_ptr);

    detector_pos_checks->setLayout(detector_pos_grid);
    detector_pos_checks->setSizePolicy(*_size_policy_box);

    QGroupBox* ki_checks = new QGroupBox("Incident wavevector");
    QGridLayout* ki_grid = new QGridLayout();

    check_ptr = new PlotCheckBox("x", TableType::Ki, 1);
    check_ptr->setMaximumWidth(1000);
    check_ptr->setSizePolicy(*_size_policy_widgets);
    ki_grid->addWidget(check_ptr, 0, 0, 1, 1);
    _plot_check_boxes.push_back(check_ptr);
    check_ptr = new PlotCheckBox("y", TableType::Ki, 2);
    check_ptr->setMaximumWidth(1000);
    check_ptr->setSizePolicy(*_size_policy_widgets);
    ki_grid->addWidget(check_ptr, 0, 1, 1, 1);
    _plot_check_boxes.push_back(check_ptr);
    check_ptr = new PlotCheckBox("z", TableType::Ki, 3);
    check_ptr->setMaximumWidth(1000);
    check_ptr->setSizePolicy(*_size_policy_widgets);
    ki_grid->addWidget(check_ptr, 0, 2, 1, 1);
    _plot_check_boxes.push_back(check_ptr);

    ki_checks->setLayout(ki_grid);
    ki_checks->setSizePolicy(*_size_policy_box);

    plot_layout->addWidget(lattice_checks);
    plot_layout->addWidget(sample_pos_checks);
    plot_layout->addWidget(sample_orn_checks);
    plot_layout->addWidget(detector_pos_checks);
    plot_layout->addWidget(ki_checks);

    _plot_box->setContentLayout(*plot_layout, true);
    _plot_box->setSizePolicy(*_size_policy_box);
    _plot_box->contentArea.setSizePolicy(*_size_policy_box);

    _plot_box->toggler(true);
    _left_layout->addWidget(_plot_box);

    // refresh the plot whenever a PlotCheckBox state changes
    for (auto checkbox : _plot_check_boxes)
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

    for (PlotCheckBox* check : _plot_check_boxes) {
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
    _update_box = new Spoiler("4. Update predictions");

    QGridLayout* update_grid = new QGridLayout();
    QLabel* peaks_label = new QLabel("Predicted peaks");
    _predicted_combo = new QComboBox();
    _update_button = new QPushButton("Update");
    _build_shape_lib = new QPushButton("Build shape collection");

    peaks_label->setAlignment(Qt::AlignRight);
    peaks_label->setSizePolicy(*_size_policy_widgets);

    _predicted_combo->setMaximumWidth(1000);
    _predicted_combo->setSizePolicy(*_size_policy_widgets);

    _update_button->setMaximumWidth(1000);
    _update_button->setSizePolicy(*_size_policy_widgets);

    _build_shape_lib->setMaximumWidth(1000);
    _build_shape_lib->setSizePolicy(*_size_policy_widgets);

    update_grid->addWidget(peaks_label, 0, 0, 1, 1);
    update_grid->addWidget(_predicted_combo, 0, 1, 1, 1);
    update_grid->addWidget(_update_button, 1, 0, 1, 2);
    update_grid->addWidget(_build_shape_lib, 2, 0, 1, 2);

    _update_box->setContentLayout(*update_grid, true);
    _update_box->setSizePolicy(*_size_policy_box);
    _update_box->contentArea.setSizePolicy(*_size_policy_box);

    _left_layout->addWidget(_update_box);

    connect(_update_button, &QPushButton::clicked, this, &SubframeRefiner::updatePredictions);
    connect(_build_shape_lib, &QPushButton::clicked, this, &SubframeRefiner::openShapeBuilder);
    _update_box->toggler(true);
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

void SubframeRefiner::setReintegrateUp()
{
    _reintegrate_box = new Spoiler("5. Reintegrate peaks");

    QGridLayout* reintegrate_grid = new QGridLayout();
    QLabel* label_ptr;

    label_ptr = new QLabel("Peak end:");
    label_ptr->setAlignment(Qt::AlignRight);
    reintegrate_grid->addWidget(label_ptr, 3, 0, 1, 1);
    label_ptr->setSizePolicy(*_size_policy_widgets);

    label_ptr = new QLabel("Bkg begin:");
    label_ptr->setAlignment(Qt::AlignRight);
    reintegrate_grid->addWidget(label_ptr, 4, 0, 1, 1);
    label_ptr->setSizePolicy(*_size_policy_widgets);

    label_ptr = new QLabel("Bkg end:");
    label_ptr->setAlignment(Qt::AlignRight);
    reintegrate_grid->addWidget(label_ptr, 5, 0, 1, 1);
    label_ptr->setSizePolicy(*_size_policy_widgets);

    label_ptr = new QLabel("Search radius:");
    label_ptr->setAlignment(Qt::AlignRight);
    reintegrate_grid->addWidget(label_ptr, 6, 0, 1, 1);
    label_ptr->setSizePolicy(*_size_policy_widgets);

    label_ptr = new QLabel("N. of frames:");
    label_ptr->setAlignment(Qt::AlignRight);
    reintegrate_grid->addWidget(label_ptr, 7, 0, 1, 1);
    label_ptr->setSizePolicy(*_size_policy_widgets);

    _integrator_combo = new QComboBox();
    _fit_center = new QCheckBox("Fit the center");
    _fit_covariance = new QCheckBox("Fit the covariance");
    _peak_end_int = new QDoubleSpinBox();
    _bkg_start_int = new QDoubleSpinBox();
    _bkg_end_int = new QDoubleSpinBox();
    _radius_int = new QDoubleSpinBox();
    _n_frames_int = new QDoubleSpinBox();
    _reintegrate_found = new QPushButton("Reintegrate found peaks");
    _reintegrate_predicted = new QPushButton("Reintegrate predicted peaks");

    _integrator_combo->setMaximumWidth(1000);
    _integrator_combo->addItem("Pixel sum integrator");
    _integrator_combo->addItem("Gaussian integrator");
    _integrator_combo->addItem("I/Sigma integrator");
    _integrator_combo->addItem("1d profile integrator");
    _integrator_combo->addItem("3d profile integrator");
    _integrator_combo->setSizePolicy(*_size_policy_widgets);

    _fit_center->setMaximumWidth(1000);
    _fit_center->setChecked(_refiner_params.fit_center);

    _fit_covariance->setMaximumWidth(1000);
    _fit_covariance->setChecked(_refiner_params.fit_cov);

    _peak_end_int->setMaximumWidth(1000);
    _peak_end_int->setMaximum(100000);
    _peak_end_int->setDecimals(2);
    _peak_end_int->setValue(_refiner_params.peak_end);

    _bkg_start_int->setMaximumWidth(1000);
    _bkg_start_int->setMaximum(100000);
    _bkg_start_int->setDecimals(2);
    _bkg_start_int->setValue(_refiner_params.bkg_begin);

    _bkg_end_int->setMaximumWidth(1000);
    _bkg_end_int->setMaximum(100000);
    _bkg_end_int->setDecimals(2);
    _bkg_end_int->setValue(_refiner_params.bkg_end);

    _radius_int->setMaximumWidth(1000);
    _radius_int->setMaximum(100000);
    _radius_int->setDecimals(2);
    _radius_int->setValue(_refiner_params.neighbour_range_pixels);

    _n_frames_int->setMaximumWidth(1000);
    _n_frames_int->setMaximum(100000);
    _n_frames_int->setDecimals(2);
    _n_frames_int->setValue(_refiner_params.neighbour_range_frames);

    _reintegrate_found->setMaximumWidth(1000);

    _reintegrate_predicted->setMaximumWidth(1000);

    _integrator_combo->setSizePolicy(*_size_policy_widgets);
    _fit_center->setSizePolicy(*_size_policy_widgets);
    _fit_covariance->setSizePolicy(*_size_policy_widgets);
    _peak_end_int->setSizePolicy(*_size_policy_widgets);
    _bkg_start_int->setSizePolicy(*_size_policy_widgets);
    _bkg_end_int->setSizePolicy(*_size_policy_widgets);
    _radius_int->setSizePolicy(*_size_policy_widgets);
    _n_frames_int->setSizePolicy(*_size_policy_widgets);
    _reintegrate_found->setSizePolicy(*_size_policy_widgets);
    _reintegrate_predicted->setSizePolicy(*_size_policy_widgets);

    reintegrate_grid->addWidget(_integrator_combo, 0, 0, 1, 2);
    reintegrate_grid->addWidget(_fit_center, 1, 0, 1, 2);
    reintegrate_grid->addWidget(_fit_covariance, 2, 0, 1, 2);
    reintegrate_grid->addWidget(_peak_end_int, 3, 1, 1, 1);
    reintegrate_grid->addWidget(_bkg_start_int, 4, 1, 1, 1);
    reintegrate_grid->addWidget(_bkg_end_int, 5, 1, 1, 1);
    reintegrate_grid->addWidget(_radius_int, 6, 1, 1, 1);
    reintegrate_grid->addWidget(_n_frames_int, 7, 1, 1, 1);
    reintegrate_grid->addWidget(_reintegrate_found, 8, 0, 1, 2);
    reintegrate_grid->addWidget(_reintegrate_predicted, 9, 0, 1, 2);

    _reintegrate_box->setContentLayout(*reintegrate_grid, true);
    _reintegrate_box->setSizePolicy(*_size_policy_box);
    _reintegrate_box->contentArea.setSizePolicy(*_size_policy_box);

    connect(_reintegrate_found, &QPushButton::clicked, this, &SubframeRefiner::reintegrateFound);
    connect(
        _reintegrate_predicted, &QPushButton::clicked, this,
        &SubframeRefiner::reintegratePredicted);
    _reintegrate_box->toggler(true);

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
    nsx::Experiment* expt = gSession->experimentAt(_exp_combo->currentIndex())->experiment();
    nsx::PeakCollection* found_peaks =
        expt->getPeakCollection(_peak_combo->currentText().toStdString());
    runReintegration(found_peaks);
}

void SubframeRefiner::reintegratePredicted()
{
    nsx::Experiment* expt = gSession->experimentAt(_exp_combo->currentIndex())->experiment();
    nsx::PeakCollection* predicted_peaks =
        expt->getPeakCollection(_predicted_combo->currentText().toStdString());
    runReintegration(predicted_peaks);
}

void SubframeRefiner::openShapeBuilder()
{
    nsx::PeakCollection* peak_collection =
        gSession->experimentAt(_exp_combo->currentIndex())
        ->experiment()
        ->getPeakCollection(_peak_combo->currentText().toStdString());

    std::unique_ptr<ShapeCollectionDialog> dialog(new ShapeCollectionDialog(peak_collection, _shape_params));

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
