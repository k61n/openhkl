//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/subframe_filter/SubframeFilterPeaks.cpp
//! @brief     Implements class SubframeFilterPeaks
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "gui/subframe_filter/SubframeFilterPeaks.h"
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

SubframeFilterPeaks::SubframeFilterPeaks()
    : QWidget()
    ,
    // _pixmap(nullptr),
    _peak_collection("temp", nsx::listtype::FOUND)
    , _peak_collection_item()
    , _peak_collection_model()
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
    setStateUp();
    setUnitCellUp();
    setStrengthUp();
    setRangeUp();
    setSparseUp();
    setMergeUp();
    setProceedUp();
    setFigureUp();
    setPeakTableUp();

    _left_layout->addItem(new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding));
    _right_element->setSizePolicy(*_size_policy_right);

    _main_layout->addWidget(scroll_area);
    _main_layout->addWidget(_right_element);
}

SubframeFilterPeaks::~SubframeFilterPeaks()
{
    delete _size_policy_widgets;
    delete _size_policy_box;
    delete _size_policy_right;
    delete _size_policy_fixed;
}

void SubframeFilterPeaks::setSizePolicies()
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

void SubframeFilterPeaks::setInputUp()
{
    _input_box = new Spoiler(QString::fromStdString("Input"));

    QGridLayout* _input_grid = new QGridLayout();

    QLabel* exp_label = new QLabel("Experiment");
    exp_label->setAlignment(Qt::AlignRight);
    _input_grid->addWidget(exp_label, 0, 0, 1, 1);

    QLabel* list_label = new QLabel("Data-set");
    list_label->setAlignment(Qt::AlignRight);
    _input_grid->addWidget(list_label, 1, 0, 1, 1);

    _exp_combo = new QComboBox();
    _peak_combo = new QComboBox();

    _exp_combo->setMaximumWidth(1000);
    _peak_combo->setMaximumWidth(1000);

    _exp_combo->setSizePolicy(*_size_policy_widgets);
    _peak_combo->setSizePolicy(*_size_policy_widgets);

    _input_grid->addWidget(_exp_combo, 0, 1, 1, 1);
    _input_grid->addWidget(_peak_combo, 1, 1, 1, 1);

    connect(
        _exp_combo, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this,
        [=]() {
            updatePeakList();
            grabFilterParameters();
        });

    connect(
        _peak_combo, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this,
        [=]() {
            updateDatasetList();
            refreshPeakTable();
        });

    _input_box->setContentLayout(*_input_grid, true);
    _input_box->setSizePolicy(*_size_policy_box);
    _input_box->contentArea.setSizePolicy(*_size_policy_box);

    _left_layout->addWidget(_input_box);
}

void SubframeFilterPeaks::setStateUp()
{
    _state_box = new SpoilerCheck("State");

    QGridLayout* _state_box_layout = new QGridLayout();

    _selected = new QCheckBox("Selected");
    _masked = new QCheckBox("Masked");
    _predicted = new QCheckBox("Predicted");
    _indexed_peaks = new QCheckBox("Indexed peak");

    _selected->setMaximumWidth(1000);
    _masked->setMaximumWidth(1000);
    _predicted->setMaximumWidth(1000);
    _indexed_peaks->setMaximumWidth(1000);

    _selected->setSizePolicy(*_size_policy_widgets);
    _masked->setSizePolicy(*_size_policy_widgets);
    _predicted->setSizePolicy(*_size_policy_widgets);
    _indexed_peaks->setSizePolicy(*_size_policy_widgets);

    _state_box_layout->addWidget(_selected, 0, 0, 1, 2);
    _state_box_layout->addWidget(_masked, 1, 0, 1, 2);
    _state_box_layout->addWidget(_predicted, 2, 0, 1, 2);
    _state_box_layout->addWidget(_indexed_peaks, 3, 0, 1, 2);

    _state_box->setContentLayout(*_state_box_layout);
    _state_box->setSizePolicy(*_size_policy_box);
    _state_box->contentArea.setSizePolicy(*_size_policy_box);

    _left_layout->addWidget(_state_box);
}

void SubframeFilterPeaks::setUnitCellUp()
{
    _unit_cell_box = new SpoilerCheck("Indexed peaks by unit cell");

    QGridLayout* _unit_cell_layout = new QGridLayout();

    QLabel* unit_cell_label = new QLabel("Unit cell:");
    unit_cell_label->setAlignment(Qt::AlignRight);
    _unit_cell_layout->addWidget(unit_cell_label, 0, 0, 1, 1);

    QLabel* tolerance_label = new QLabel("Tolerance:");
    tolerance_label->setAlignment(Qt::AlignRight);
    _unit_cell_layout->addWidget(tolerance_label, 1, 0, 1, 1);

    _unit_cell = new QComboBox();

    _tolerance = new QDoubleSpinBox();
    _tolerance->setValue(0.2);
    _tolerance->setMaximum(1000);
    _tolerance->setDecimals(6);

    _unit_cell->setSizePolicy(*_size_policy_widgets);
    _tolerance->setSizePolicy(*_size_policy_widgets);

    _unit_cell_layout->addWidget(_unit_cell, 0, 1, 1, 1);
    _unit_cell_layout->addWidget(_tolerance, 1, 1, 1, 1);

    _unit_cell_box->setContentLayout(*_unit_cell_layout);
    _unit_cell_box->setSizePolicy(*_size_policy_box);
    _unit_cell_box->contentArea.setSizePolicy(*_size_policy_box);

    _left_layout->addWidget(_unit_cell_box);
}

void SubframeFilterPeaks::setStrengthUp()
{
    _strength_box = new SpoilerCheck("Strength (I/sigma)");

    QGridLayout* strength_layout = new QGridLayout();

    QLabel* min_label = new QLabel("Minimum:");
    min_label->setAlignment(Qt::AlignRight);
    strength_layout->addWidget(min_label, 0, 0, 1, 1);

    QLabel* max_label = new QLabel("Maximum:");
    max_label->setAlignment(Qt::AlignRight);
    strength_layout->addWidget(max_label, 1, 0, 1, 1);

    _strength_min = new QDoubleSpinBox();
    _strength_min->setValue(1.00000);
    _strength_min->setMaximum(100000);
    _strength_min->setDecimals(6);

    _strength_max = new QDoubleSpinBox();
    _strength_max->setValue(3.00000);
    _strength_max->setMaximum(1e10);
    _strength_max->setDecimals(6);

    _strength_min->setSizePolicy(*_size_policy_widgets);
    _strength_max->setSizePolicy(*_size_policy_widgets);

    strength_layout->addWidget(_strength_min, 0, 1, 1, 1);
    strength_layout->addWidget(_strength_max, 1, 1, 1, 1);

    _strength_box->setContentLayout(*strength_layout);
    _strength_box->setSizePolicy(*_size_policy_box);
    _strength_box->contentArea.setSizePolicy(*_size_policy_box);

    _left_layout->addWidget(_strength_box);
}

void SubframeFilterPeaks::setRangeUp()
{
    _d_range_box = new SpoilerCheck("d range");

    QGridLayout* d_range_layout = new QGridLayout();

    QLabel* min_label = new QLabel("Minimum:");
    min_label->setAlignment(Qt::AlignRight);
    d_range_layout->addWidget(min_label, 0, 0, 1, 1);

    QLabel* max_label = new QLabel("Maximum:");
    max_label->setAlignment(Qt::AlignRight);
    d_range_layout->addWidget(max_label, 1, 0, 1, 1);

    _d_range_min = new QDoubleSpinBox();
    _d_range_min->setMaximum(1000);
    _d_range_min->setDecimals(6);
    _d_range_min->setValue(0.0000);

    _d_range_max = new QDoubleSpinBox();
    _d_range_max->setMaximum(1000);
    _d_range_max->setDecimals(6);
    _d_range_max->setValue(100.00000);

    _d_range_min->setSizePolicy(*_size_policy_widgets);
    _d_range_max->setSizePolicy(*_size_policy_widgets);

    d_range_layout->addWidget(_d_range_min, 0, 1, 1, 1);
    d_range_layout->addWidget(_d_range_max, 1, 1, 1, 1);

    _d_range_box->setContentLayout(*d_range_layout);
    _d_range_box->setSizePolicy(*_size_policy_box);
    _d_range_box->contentArea.setSizePolicy(*_size_policy_box);

    _left_layout->addWidget(_d_range_box);
}

void SubframeFilterPeaks::setSparseUp()
{
    _sparse_box = new SpoilerCheck("Sparse dataset");

    QGridLayout* sparse_layout = new QGridLayout();

    QLabel* min_label = new QLabel("Minimum number of peaks:");
    min_label->setAlignment(Qt::AlignRight);
    sparse_layout->addWidget(min_label, 0, 0, 1, 1);

    _min_number_peaks = new QSpinBox();
    _min_number_peaks->setValue(0);
    _min_number_peaks->setMaximum(1000);

    _min_number_peaks->setSizePolicy(*_size_policy_widgets);

    sparse_layout->addWidget(_min_number_peaks, 0, 1, 1, 1);

    _sparse_box->setContentLayout(*sparse_layout);
    _sparse_box->setSizePolicy(*_size_policy_box);
    _sparse_box->contentArea.setSizePolicy(*_size_policy_box);

    _left_layout->addWidget(_sparse_box);
}

void SubframeFilterPeaks::setMergeUp()
{
    _merge_box = new SpoilerCheck("Merged peak significance");

    QGridLayout* merge_layout = new QGridLayout();

    QLabel* significance_label = new QLabel("Significant level:");
    significance_label->setAlignment(Qt::AlignRight);
    merge_layout->addWidget(significance_label, 0, 0, 1, 1);

    _significance_level = new QDoubleSpinBox();
    _significance_level->setValue(0.990000);
    _significance_level->setMaximum(1000);
    _significance_level->setDecimals(6);

    _significance_level->setSizePolicy(*_size_policy_widgets);

    merge_layout->addWidget(_significance_level, 0, 1, 1, 1);

    _merge_box->setContentLayout(*merge_layout);
    _merge_box->setSizePolicy(*_size_policy_box);
    _merge_box->contentArea.setSizePolicy(*_size_policy_box);

    _left_layout->addWidget(_merge_box);
}

void SubframeFilterPeaks::setProceedUp()
{
    _remove_overlaping = new QCheckBox("Remove overlapping peaks");
    _remove_overlaping->setChecked(false);
    _remove_overlaping->setSizePolicy(*_size_policy_widgets);
    _left_layout->addWidget(_remove_overlaping);

    _extinct_spacegroup = new QCheckBox("Remove extinct from spacegroup");
    _extinct_spacegroup->setChecked(false);
    _extinct_spacegroup->setSizePolicy(*_size_policy_widgets);
    _left_layout->addWidget(_extinct_spacegroup);

    _keep_complementary = new QCheckBox("Keep the complementary selection");
    _keep_complementary->setChecked(false);
    _keep_complementary->setSizePolicy(*_size_policy_widgets);
    _left_layout->addWidget(_keep_complementary);

    _filter_button = new QPushButton("Filter");
    _filter_button->setSizePolicy(*_size_policy_widgets);
    _left_layout->addWidget(_filter_button);

    _save_button = new QPushButton("Save");
    _save_button->setSizePolicy(*_size_policy_widgets);
    _left_layout->addWidget(_save_button);

    connect(_filter_button, &QPushButton::clicked, this, &SubframeFilterPeaks::filterPeaks);

    connect(_save_button, &QPushButton::clicked, this, &SubframeFilterPeaks::accept);
}

void SubframeFilterPeaks::setFigureUp()
{
    QGroupBox* figure_group = new QGroupBox("Preview");
    QGridLayout* figure_grid = new QGridLayout(figure_group);

    figure_group->setSizePolicy(*_size_policy_right);

    _figure_view = new DetectorView(this);
    _figure_view->getScene()->linkPeakModel(&_peak_collection_model);
    _figure_view->scale(1, -1);
    figure_grid->addWidget(_figure_view, 0, 0, 1, 3);

    _data_combo = new QComboBox(this);
    _data_combo->setSizePolicy(*_size_policy_widgets);
    figure_grid->addWidget(_data_combo, 1, 0, 1, 1);

    _figure_scroll = new QScrollBar(this);
    _figure_scroll->setOrientation(Qt::Horizontal);
    _figure_scroll->setSizePolicy(*_size_policy_widgets);
    figure_grid->addWidget(_figure_scroll, 1, 1, 1, 1);

    _figure_spin = new QSpinBox(this);
    _figure_spin->setSizePolicy(*_size_policy_fixed);
    figure_grid->addWidget(_figure_spin, 1, 2, 1, 1);

    connect(
        _figure_scroll, SIGNAL(valueChanged(int)), _figure_view->getScene(),
        SLOT(slotChangeSelectedFrame(int)));

    connect(_figure_scroll, SIGNAL(valueChanged(int)), _figure_spin, SLOT(setValue(int)));

    connect(
        _figure_view->getScene(), &DetectorScene::signalSelectedPeakItemChanged, this,
        &SubframeFilterPeaks::changeSelected);

    _right_element->addWidget(figure_group);
}

void SubframeFilterPeaks::setPeakTableUp()
{
    QGroupBox* peak_group = new QGroupBox("Peaks");
    QGridLayout* peak_grid = new QGridLayout(peak_group);

    _preview_panel = peak_group;
    peak_group->setSizePolicy(*_size_policy_right);

    _peak_table = new PeaksTableView(this);
    _peak_collection_model.setRoot(&_peak_collection_item);
    _peak_table->setModel(&_peak_collection_model);

    peak_grid->addWidget(_peak_table, 0, 0, 0, 0);

    _right_element->addWidget(peak_group);
}

void SubframeFilterPeaks::refreshAll()
{
    setParametersUp();
}

void SubframeFilterPeaks::setParametersUp()
{
    setExperimentsUp();
}

void SubframeFilterPeaks::setExperimentsUp()
{
    _exp_combo->blockSignals(true);
    _exp_combo->clear();

    if (gSession->experimentNames().empty())
        return;

    for (const QString& exp : gSession->experimentNames())
        _exp_combo->addItem(exp);

    _exp_combo->blockSignals(false);
    updatePeakList();
    grabFilterParameters();
}

void SubframeFilterPeaks::updatePeakList()
{
    _peak_combo->blockSignals(true);

    _peak_combo->clear();
    _peak_list = gSession->experimentAt(_exp_combo->currentIndex())->getPeakListNames();

    if (!_peak_list.empty()) {
        _peak_combo->addItems(_peak_list);
        _peak_combo->setCurrentIndex(0);

        nsx::PeakFilter* filter =
            gSession->experimentAt(_exp_combo->currentIndex())->experiment()->peakFilter();
        nsx::PeakCollection* collection =
            gSession->experimentAt(_exp_combo->currentIndex())
                ->experiment()
                ->getPeakCollection(_peak_combo->currentText().toStdString());
        filter->resetFiltering(collection);

        updateDatasetList();
        refreshPeakTable();
    }
    _peak_combo->blockSignals(false);
}

void SubframeFilterPeaks::updateDatasetList()
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
        updateDatasetParameters(0);
    }
    _data_combo->blockSignals(false);
}

void SubframeFilterPeaks::updateDatasetParameters(int idx)
{
    if (_data_list.empty() || idx < 0)
        return;

    nsx::sptrDataSet data = _data_list.at(idx);

    _figure_view->getScene()->slotChangeSelectedData(_data_list.at(idx), 0);
    _figure_view->getScene()->setMaxIntensity(3000);
    emit _figure_view->getScene()->dataChanged();
    _figure_view->getScene()->update();

    _figure_scroll->setMaximum(data->nFrames());
    _figure_scroll->setMinimum(0);

    _figure_spin->setMaximum(data->nFrames());
    _figure_spin->setMinimum(0);
}

void SubframeFilterPeaks::grabFilterParameters()
{
    if (_peak_list.empty() || _exp_combo->count() < 1)
        return;

    nsx::PeakFilter* filter =
        gSession->experimentAt(_exp_combo->currentIndex())->experiment()->peakFilter();

    _tolerance->setValue(*(filter->unitCellTolerance()));
    _strength_min->setValue(filter->strength()->at(0));
    _strength_max->setValue(filter->strength()->at(1));
    _d_range_min->setValue(filter->dRange()->at(0));
    _d_range_max->setValue(filter->dRange()->at(1));
    _significance_level->setValue(*(filter->significance()));

    _selected->setChecked(filter->getFilterSelected());
    _masked->setChecked(filter->getFilterMasked());
    _predicted->setChecked(filter->getFilterPredicted());
    _indexed_peaks->setChecked(filter->getFilterIndexed());
    _extinct_spacegroup->setChecked(filter->getFilterExtinct());
    _remove_overlaping->setChecked(filter->getFilterOverlapping());
    _keep_complementary->setChecked(filter->getFilterComplementary());

    _state_box->checker(filter->getFilterState());
    _unit_cell_box->checker(filter->getFilterIndexTol());
    _strength_box->checker(filter->getFilterStrength());
    _d_range_box->checker(filter->getFilterDRange());
    _sparse_box->checker(filter->getFilterSparse());
    _merge_box->checker(filter->getFilterSignificance());
}

void SubframeFilterPeaks::setFilterParameters() const
{
    if (_peak_list.empty() || _exp_combo->count() < 1)
        return;

    nsx::PeakFilter* filter =
        gSession->experimentAt(_exp_combo->currentIndex())->experiment()->peakFilter();
    filter->resetFilterFlags();

    if (_selected->isChecked())
        filter->setFilterSelected(true);
    if (_masked->isChecked())
        filter->setFilterMasked(true);
    if (_predicted->isChecked())
        filter->setFilterPredicted(true);
    if (_indexed_peaks->isChecked())
        filter->setFilterIndexed(true);
    if (_extinct_spacegroup->isChecked())
        filter->setFilterExtinct(true);
    if (_remove_overlaping->isChecked())
        filter->setFilterOverlapping(true);
    if (_keep_complementary->isChecked())
        filter->setFilterComplementary(true);

    if (_state_box->checked())
        filter->setFilterState(true);
    if (_unit_cell_box->checked())
        filter->setFilterIndexTol(true);
    if (_strength_box->checked())
        filter->setFilterStrength(true);
    if (_d_range_box->checked())
        filter->setFilterDRange(true);
    if (_sparse_box->checked())
        filter->setFilterSparse(true);
    if (_merge_box->checked())
        filter->setFilterSignificance(true);

    const std::array<double, 2> d_range{_d_range_min->value(), _d_range_max->value()};
    const std::array<double, 2> strength{_strength_min->value(), _strength_max->value()};

    filter->setUnitCellTolerance(_tolerance->value());
    filter->setSignificance(_significance_level->value());
    filter->setDRange(d_range);
    filter->setStrength(strength);
    filter->setUnitCellName(_unit_cell->currentText().toStdString());
}

void SubframeFilterPeaks::filterPeaks()
{
    if (_peak_list.empty() || _exp_combo->count() < 1)
        return;

    setFilterParameters();

    nsx::PeakFilter* filter =
        gSession->experimentAt(_exp_combo->currentIndex())->experiment()->peakFilter();
    nsx::PeakCollection* collection =
        gSession->experimentAt(_exp_combo->currentIndex())
            ->experiment()
            ->getPeakCollection(_peak_combo->currentText().toStdString());
    filter->resetFiltering(collection);
    filter->filter(collection);

    int n_peaks = _peak_collection_item.numberOfPeaks();
    int n_caught = _peak_collection_item.numberCaughtByFilter();

    std::ostringstream oss;
    oss << n_caught << " of " << n_peaks << " peaks caught by filter";
    QString new_title = QString::fromUtf8(oss.str().c_str());
    std::cout << n_caught << " of " << n_peaks << " peaks caught by filter" << std::endl;
    _preview_panel->setTitle(new_title);

    refreshPeakTable();
}

void SubframeFilterPeaks::accept()
{
    if (_peak_list.empty() || _exp_combo->count() < 1)
        return;
    nsx::PeakCollection* collection =
        gSession->experimentAt(_exp_combo->currentIndex())
            ->experiment()
            ->getPeakCollection(_peak_combo->currentText().toStdString());

    std::unique_ptr<ListNameDialog> dlg(new ListNameDialog());
    dlg->exec();
    if (!dlg->listName().isEmpty()) {
        gSession->experimentAt(_exp_combo->currentIndex())
            ->experiment()
            ->acceptFilter(dlg->listName().toStdString(), collection);
        gSession->experimentAt(_exp_combo->currentIndex())->generatePeakModel(dlg->listName());
    }
}

void SubframeFilterPeaks::refreshPeakTable()
{
    if (_peak_list.empty() || _exp_combo->count() < 1)
        return;

    _figure_view->getScene()->clearPeakItems();
    nsx::PeakCollection* collection =
        gSession->experimentAt(_exp_combo->currentIndex())
            ->experiment()
            ->getPeakCollection(_peak_combo->currentText().toStdString());
    _peak_collection_item.setPeakCollection(collection);
    _peak_collection_item.setFilterMode();
    _peak_collection_model.setRoot(&_peak_collection_item);

    refreshPeakVisual();
}

void SubframeFilterPeaks::refreshPeakVisual()
{
    if (_peak_collection_item.childCount() == 0)
        return;

    bool caught;
    PeakItemGraphic* graphic;

    for (int i = 0; i < _peak_collection_item.childCount(); ++i) {
        PeakItem* peak = _peak_collection_item.peakItemAt(i);
        graphic = peak->peakGraphic();
        caught = peak->peak()->caughtByFilter();

        if (caught) {
            graphic->showArea(true);
            graphic->showLabel(false);
            graphic->setSize(10);
            graphic->setColor(Qt::darkGreen);
            graphic->setOutlineColor(Qt::transparent);
        } else {
            graphic->showArea(true);
            graphic->showLabel(false);
            graphic->setSize(10);
            graphic->setColor(Qt::darkRed);
            graphic->setOutlineColor(Qt::transparent);
        }
    }
    _figure_view->getScene()->update();
    _figure_view->getScene()->drawPeakitems();
}

void SubframeFilterPeaks::changeSelected(PeakItemGraphic* peak_graphic)
{
    int row = _peak_collection_item.returnRowOfVisualItem(peak_graphic);
    QModelIndex index = _peak_collection_model.index(row, 0);
    _peak_table->selectRow(row);
    _peak_table->scrollTo(index, QAbstractItemView::PositionAtTop);
}
