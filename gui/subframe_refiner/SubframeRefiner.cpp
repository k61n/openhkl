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
#include "gui/models/Meta.h"
#include "gui/models/Project.h"
#include "gui/models/Session.h"
#include <QFileInfo>
#include <QGridLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QItemDelegate>
#include <QLabel>
#include <QScrollArea>
#include <QScrollBar>
#include <QSpacerItem>
#include <QTableWidgetItem>
#include <sstream>

SubframeRefiner::SubframeRefiner()
    : QWidget()
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

    setInputUp();
    setRefinerFlagsUp();
    setUpdateUp();

    _left_layout->addItem(new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding));
    _right_element->setSizePolicy(*_size_policy_right);

    _main_layout->addWidget(scroll_area);
    _main_layout->addWidget(_right_element);
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
    _size_policy_box->setVerticalPolicy(QSizePolicy::Preferred);

    _size_policy_right = new QSizePolicy();
    _size_policy_right->setHorizontalPolicy(QSizePolicy::Expanding);
    _size_policy_right->setVerticalPolicy(QSizePolicy::Expanding);

    _size_policy_fixed = new QSizePolicy();
    _size_policy_fixed->setHorizontalPolicy(QSizePolicy::Fixed);
    _size_policy_fixed->setVerticalPolicy(QSizePolicy::Fixed);
}

void SubframeRefiner::setInputUp()
{
    _input_box = new Spoiler("Input");

    QGridLayout* _input_grid = new QGridLayout();

    QLabel* exp_label = new QLabel("Experiment");
    exp_label->setAlignment(Qt::AlignRight);
    _input_grid->addWidget(exp_label, 0, 0, 1, 1);

    QLabel* peak_label = new QLabel("Peak collection");
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

    _n_batches_spin->setValue(1);
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
        &SubframeRefiner::updatePeakList);
    connect(
        _peak_combo, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this,
        &SubframeRefiner::updateDatasetList);
    // connect(_data_combo)
    // connect(_peak_combo)
    // connect(_cell_combo)

    _input_box->setContentLayout(*_input_grid, true);
    _input_box->setSizePolicy(*_size_policy_box);
    _input_box->contentArea.setSizePolicy(*_size_policy_box);

    _left_layout->addWidget(_input_box);
}

void SubframeRefiner::setRefinerFlagsUp()
{
    _refiner_flags_box = new Spoiler("Parameters to refine");

    QGridLayout* refiner_flags_layout = new QGridLayout();
    _refineUB = new QCheckBox("Cell vectors");
    _refineSamplePosition = new QCheckBox("Sample position");
    _refineSampleOrientation = new QCheckBox("Sample orientation");
    _refineDetectorOrientation = new QCheckBox("Detector orientation");
    _refineKi = new QCheckBox("Incident wavevector");

    _refineUB->setMaximumWidth(1000);
    _refineSamplePosition->setMaximumWidth(1000);
    _refineSampleOrientation->setMaximumWidth(1000);
    _refineDetectorOrientation->setMaximumWidth(1000);
    _refineKi->setMaximumWidth(1000);

    _refineUB->setSizePolicy(*_size_policy_widgets);
    _refineSamplePosition->setSizePolicy(*_size_policy_widgets);
    _refineSampleOrientation->setSizePolicy(*_size_policy_widgets);
    _refineDetectorOrientation->setSizePolicy(*_size_policy_widgets);
    _refineKi->setSizePolicy(*_size_policy_widgets);

    _refine_button = new QPushButton("Refine");
    _refine_button->setSizePolicy(*_size_policy_widgets);

    refiner_flags_layout->addWidget(_refineUB, 0, 0, 1, 2);
    refiner_flags_layout->addWidget(_refineSamplePosition, 1, 0, 1, 2);
    refiner_flags_layout->addWidget(_refineSampleOrientation, 2, 0, 1, 2);
    refiner_flags_layout->addWidget(_refineDetectorOrientation, 3, 0, 1, 2);
    refiner_flags_layout->addWidget(_refineKi, 4, 0, 1, 2);
    refiner_flags_layout->addWidget(_refine_button, 5, 0, 1, 2);

    _refiner_flags_box->setContentLayout(*refiner_flags_layout);
    _refiner_flags_box->setSizePolicy(*_size_policy_box);
    _refiner_flags_box->contentArea.setSizePolicy(*_size_policy_box);

    _refineUB->setChecked(true);
    _refineSamplePosition->setChecked(true);
    _refineSampleOrientation->setChecked(true);
    _refineDetectorOrientation->setChecked(true);
    _refineKi->setChecked(true);


    connect(_refine_button, &QPushButton::clicked, this, &SubframeRefiner::refine);

    _left_layout->addWidget(_refiner_flags_box);
}


void SubframeRefiner::setRefinerTableUp()
{
    QGroupBox* peak_group = new QGroupBox("Refiner");
    // QGridLayout* peak_grid = new QGridLayout(peak_group);

    // _preview_panel = peak_group;
    // peak_group->setSizePolicy(*_size_policy_right);

    // _peak_table = new PeaksTableView(this);
    // _peak_collection_model.setRoot(&_peak_collection_item);
    // _peak_table->setModel(&_peak_collection_model);

    // peak_grid->addWidget(_peak_table, 0, 0, 0, 0);

    // _right_element->addWidget(peak_group);
}

void SubframeRefiner::refreshAll()
{
    updateExptList();
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
}

void SubframeRefiner::updateDatasetList()
{
    _data_combo->blockSignals(true);
    _data_combo->clear();

    _data_list = gSession->experimentAt(_exp_combo->currentIndex())->allData();

    if (!_data_list.empty()) {
        for (const nsx::sptrDataSet& data : _data_list) {
            QFileInfo fileinfo(QString::fromStdString(data->filename()));
            _data_combo->addItem(fileinfo.baseName());
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

    _peak_list = gSession->experimentAt(_exp_combo->currentIndex())->getPeakListNames();

    if (!_peak_list.empty()) {
        _peak_combo->addItems(_peak_list);
        _peak_combo->setCurrentIndex(0);
    }
    _peak_combo->blockSignals(false);

    updatePredictedList();
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
    auto dataset =
        gSession->experimentAt(_exp_combo->currentIndex())->getData(_data_combo->currentIndex());
    _n_batches_spin->setMaximum(dataset->nFrames());
}

void SubframeRefiner::refine()
{
    auto expt = gSession->experimentAt(_exp_combo->currentIndex())->experiment();
    auto data = expt->getData(_data_combo->currentText().toStdString());
    auto peaks = expt->getPeakCollection(_peak_combo->currentText().toStdString());
    auto cell = expt->getUnitCell(_cell_combo->currentText().toStdString());
    auto cell_handler = expt->getCellHandler();
    auto peak_list = peaks->getPeakList();
    int n_batches = _n_batches_spin->value();
    auto states = data->instrumentStates();
    const unsigned int max_iter = 1000;
    _refiner = std::make_unique<nsx::Refiner>(states, cell, peak_list, n_batches, cell_handler);
    int n_checked = 0;
    if (_refineUB->isChecked()) {
        _refiner->refineUB();
        ++n_checked;
    }
    if (_refineSamplePosition->isChecked()) {
        _refiner->refineSamplePosition();
        ++n_checked;
    }
    if (_refineSampleOrientation->isChecked()) {
        _refiner->refineSampleOrientation();
        ++n_checked;
    }
    if (_refineDetectorOrientation->isChecked()) {
        _refiner->refineDetectorOffset();
        ++n_checked;
    }
    if (_refineKi->isChecked()) {
        _refiner->refineKi();
        ++n_checked;
    }
    if (n_checked > 0) {
        bool success = _refiner->refine(max_iter);
    }
}

void SubframeRefiner::setUpdateUp()
{
    _update_box = new Spoiler("Update predictions");

    QGridLayout* update_grid = new QGridLayout();
    QLabel* peaks_label = new QLabel("Peaks");
    _predicted_combo = new QComboBox();
    _update_button = new QPushButton("Update");

    peaks_label->setAlignment(Qt::AlignRight);
    peaks_label->setSizePolicy(*_size_policy_widgets);

    _predicted_combo->setMaximumWidth(1000);
    _predicted_combo->setSizePolicy(*_size_policy_widgets);

    _update_button->setMaximumWidth(1000);
    _update_button->setSizePolicy(*_size_policy_widgets);

    update_grid->addWidget(peaks_label, 0, 0, 1, 1);
    update_grid->addWidget(_predicted_combo, 0, 1, 1, 1);
    update_grid->addWidget(_update_button, 1, 0, 1, 2);

    _update_box->setContentLayout(*update_grid, true);
    _update_box->setSizePolicy(*_size_policy_box);
    _update_box->contentArea.setSizePolicy(*_size_policy_box);

    _left_layout->addWidget(_update_box);

    connect(_update_button, &QPushButton::clicked, this, &SubframeRefiner::updatePredictions);
}

void SubframeRefiner::updatePredictedList()
{
    _predicted_combo->blockSignals(true);
    _predicted_combo->clear();

    _predicted_list =
        gSession->experimentAt(_exp_combo->currentIndex())->getPredictedNames();

    if (!_predicted_list.empty()) {
        _predicted_combo->addItems(_peak_list);
        _predicted_combo->setCurrentIndex(0);
    }
    _predicted_combo->blockSignals(false);
}

void SubframeRefiner::updatePredictions()
{
    auto expt = gSession->experimentAt(_exp_combo->currentIndex())->experiment();
    auto peaks = expt->getPeakCollection(_predicted_combo->currentText().toStdString());
    auto peak_list = peaks->getPeakList();
    _n_updated = _refiner->updatePredictions(peak_list);
}
