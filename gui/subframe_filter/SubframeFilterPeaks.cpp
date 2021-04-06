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
#include "gui/graphics/DetectorScene.h"
#include "gui/graphics/DetectorView.h"
#include "gui/models/Meta.h"
#include "gui/models/Project.h"
#include "gui/models/Session.h"
#include "gui/utility/GridFiller.h"
#include "gui/utility/PropertyScrollArea.h"
#include "gui/utility/Spoiler.h"
#include "gui/utility/SpoilerCheck.h"
#include "gui/widgets/PeakViewWidget.h"

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
    , _peak_collection("temp", nsx::listtype::FOUND)
    , _peak_collection_item()
    , _peak_collection_model()
    , _size_policy_right(QSizePolicy::Expanding, QSizePolicy::Expanding)
{
    _main_layout = new QHBoxLayout(this);
    _right_element = new QSplitter(Qt::Vertical, this);

    _left_layout = new QVBoxLayout(this);

    setInputUp();
    setStateUp();
    setUnitCellUp();
    setStrengthUp();
    setRangeUp();
    setFrameRangeUp();
    setSparseUp();
    setMergeUp();
    setProceedUp();
    setFigureUp();
    setPeakTableUp();

    _right_element->setSizePolicy(_size_policy_right);

    auto propertyScrollArea = new PropertyScrollArea(this);
    propertyScrollArea->setContentLayout(_left_layout);

    _main_layout->addWidget(propertyScrollArea);
    _main_layout->addWidget(_right_element);
}

void SubframeFilterPeaks::setInputUp()
{
    auto input_box = new Spoiler("Input");
    GridFiller f(input_box, true);

    _exp_combo = f.addCombo("Experiment");
    _peak_combo = f.addCombo("Peak collection");

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

    _left_layout->addWidget(input_box);
}

void SubframeFilterPeaks::setStateUp()
{
    _state_box = new SpoilerCheck("State");
    GridFiller f(_state_box);

    _selected = f.addCheckBox("Selected");
    _masked = f.addCheckBox("Masked");
    _predicted = f.addCheckBox("Predicted");
    _indexed_peaks = f.addCheckBox("Indexed peak");

    _left_layout->addWidget(_state_box);
}

void SubframeFilterPeaks::setUnitCellUp()
{
    _unit_cell_box = new SpoilerCheck("Indexed peaks by unit cell");
    GridFiller f(_unit_cell_box);

    _unit_cell = f.addCombo("Unit cell:");
    _tolerance = f.addDoubleSpinBox("Tolerance:");

    _tolerance->setValue(0.2);
    _tolerance->setMaximum(1000);
    _tolerance->setDecimals(6);

    _left_layout->addWidget(_unit_cell_box);
}

void SubframeFilterPeaks::setStrengthUp()
{
    _strength_box = new SpoilerCheck("Strength (I/sigma)");
    GridFiller f(_strength_box);

    _strength_min = f.addDoubleSpinBox("Minimum:");
    _strength_min->setValue(1.00000);
    _strength_min->setMaximum(100000);
    _strength_min->setDecimals(6);

    _strength_max = f.addDoubleSpinBox("Maximum:");
    _strength_max->setValue(3.00000);
    _strength_max->setMaximum(1e10);
    _strength_max->setDecimals(6);

    _left_layout->addWidget(_strength_box);
}

void SubframeFilterPeaks::setRangeUp()
{
    _d_range_box = new SpoilerCheck("d range");
    GridFiller f(_d_range_box);

    _d_range_min = f.addDoubleSpinBox("Minimum:");
    _d_range_min->setMaximum(1000);
    _d_range_min->setDecimals(6);
    _d_range_min->setValue(0.0000);

    _d_range_max = f.addDoubleSpinBox("Maximum:");
    _d_range_max->setMaximum(1000);
    _d_range_max->setDecimals(6);
    _d_range_max->setValue(100.00000);

    _left_layout->addWidget(_d_range_box);
}

void SubframeFilterPeaks::setFrameRangeUp()
{
    _frame_range_box = new SpoilerCheck("Frame range");
    GridFiller f(_frame_range_box);

    _frame_min = f.addDoubleSpinBox("Minimum:");
    _frame_min->setMaximum(10000);
    _frame_min->setDecimals(0);
    _frame_min->setValue(0.0000);

    _frame_max = f.addDoubleSpinBox("Maximum:");
    _frame_max->setMaximum(10000);
    _frame_max->setDecimals(0);
    _frame_max->setValue(10.00000);

    _left_layout->addWidget(_frame_range_box);
}

void SubframeFilterPeaks::setSparseUp()
{
    _sparse_box = new SpoilerCheck("Sparse dataset");
    GridFiller f(_sparse_box);

    _min_number_peaks = f.addSpinBox("Minimum number of peaks:");
    _min_number_peaks->setValue(0);
    _min_number_peaks->setMaximum(1000);

    _left_layout->addWidget(_sparse_box);
}

void SubframeFilterPeaks::setMergeUp()
{
    _merge_box = new SpoilerCheck("Merged peak significance");
    GridFiller f(_merge_box);

    _significance_level = f.addDoubleSpinBox("Significant level:");
    _significance_level->setValue(0.990000);
    _significance_level->setMaximum(1000);
    _significance_level->setDecimals(6);

    _left_layout->addWidget(_merge_box);
}

void SubframeFilterPeaks::setProceedUp()
{
    _remove_overlaping = new QCheckBox("Remove overlapping peaks");
    _remove_overlaping->setChecked(false);
    _left_layout->addWidget(_remove_overlaping);

    _extinct_spacegroup = new QCheckBox("Remove extinct from spacegroup");
    _extinct_spacegroup->setChecked(false);
    _left_layout->addWidget(_extinct_spacegroup);

    _keep_complementary = new QCheckBox("Keep the complementary selection");
    _keep_complementary->setChecked(false);
    _left_layout->addWidget(_keep_complementary);

    auto filter_button = new QPushButton("Filter");
    _left_layout->addWidget(filter_button);

    auto show_hide_peaks = new Spoiler("Show/hide peaks");
    _peak_view_widget = new PeakViewWidget("Peaks caught by filter", "Peaks rejected by filter");
    show_hide_peaks->setContentLayout(*_peak_view_widget);

    connect(
        _peak_view_widget, &PeakViewWidget::settingsChanged, this,
        &SubframeFilterPeaks::refreshPeakVisual);

    _left_layout->addWidget(show_hide_peaks);

    auto save_button = new QPushButton("Create peak collection");
    _left_layout->addWidget(save_button);

    connect(filter_button, &QPushButton::clicked, this, &SubframeFilterPeaks::filterPeaks);

    connect(save_button, &QPushButton::clicked, this, &SubframeFilterPeaks::accept);
}

void SubframeFilterPeaks::setFigureUp()
{
    QGroupBox* figure_group = new QGroupBox("Preview");
    QGridLayout* figure_grid = new QGridLayout(figure_group);

    figure_group->setSizePolicy(_size_policy_right);

    _figure_view = new DetectorView(this);
    _figure_view->getScene()->linkPeakModel(&_peak_collection_model);
    _figure_view->scale(1, -1);
    figure_grid->addWidget(_figure_view, 0, 0, 1, 3);

    _data_combo = new QComboBox(this);
    _data_combo->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    figure_grid->addWidget(_data_combo, 1, 0, 1, 1);

    _figure_scroll = new QScrollBar(this);
    _figure_scroll->setOrientation(Qt::Horizontal);
    _figure_scroll->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    figure_grid->addWidget(_figure_scroll, 1, 1, 1, 1);

    _figure_spin = new QSpinBox(this);
    _figure_spin->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    figure_grid->addWidget(_figure_spin, 1, 2, 1, 1);

    connect(
        _figure_scroll, SIGNAL(valueChanged(int)), _figure_view->getScene(),
        SLOT(slotChangeSelectedFrame(int)));

    connect(_figure_scroll, SIGNAL(valueChanged(int)), _figure_spin, SLOT(setValue(int)));

    connect(_figure_spin, SIGNAL(valueChanged(int)), _figure_scroll, SLOT(setValue(int)));

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
    peak_group->setSizePolicy(_size_policy_right);

    _peak_table = new PeakTableView(this);
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
    //_figure_view->getScene()->setMaxIntensity(3000);
    emit _figure_view->getScene()->dataChanged();
    _figure_view->getScene()->update();

    _figure_scroll->setMaximum(data->nFrames() - 1);
    _figure_scroll->setMinimum(0);

    _figure_spin->setMaximum(data->nFrames() - 1);
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
    _frame_min->setValue(filter->frameRange().at(0));
    _frame_max->setValue(filter->frameRange().at(1));
    _significance_level->setValue(*(filter->significance()));

    _selected->setChecked(filter->getFilterSelected());
    _masked->setChecked(filter->getFilterMasked());
    _predicted->setChecked(filter->getFilterPredicted());
    _indexed_peaks->setChecked(filter->getFilterIndexed());
    _extinct_spacegroup->setChecked(filter->getFilterExtinct());
    _remove_overlaping->setChecked(filter->getFilterOverlapping());
    _keep_complementary->setChecked(filter->getFilterComplementary());

    _state_box->setChecked(filter->getFilterState());
    _unit_cell_box->setChecked(filter->getFilterIndexTol());
    _strength_box->setChecked(filter->getFilterStrength());
    _d_range_box->setChecked(filter->getFilterDRange());
    _frame_range_box->setChecked(filter->getFilterFrames());
    _sparse_box->setChecked(filter->getFilterSparse());
    _merge_box->setChecked(filter->getFilterSignificance());
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

    if (_state_box->isChecked())
        filter->setFilterState(true);
    if (_unit_cell_box->isChecked())
        filter->setFilterIndexTol(true);
    if (_strength_box->isChecked())
        filter->setFilterStrength(true);
    if (_d_range_box->isChecked())
        filter->setFilterDRange(true);
    if (_frame_range_box->isChecked())
        filter->setFilterFrames(true);
    if (_sparse_box->isChecked())
        filter->setFilterSparse(true);
    if (_merge_box->isChecked())
        filter->setFilterSignificance(true);

    const std::array<double, 2> d_range{_d_range_min->value(), _d_range_max->value()};
    const std::array<double, 2> strength{_strength_min->value(), _strength_max->value()};
    const std::array<double, 2> frame_range{_frame_min->value(), _frame_max->value()};

    filter->setUnitCellTolerance(_tolerance->value());
    filter->setSignificance(_significance_level->value());
    filter->setDRange(d_range);
    filter->setFrameRange(frame_range);
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

    for (int i = 0; i < _peak_collection_item.childCount(); ++i) {
        PeakItem* peak = _peak_collection_item.peakItemAt(i);
        auto graphic = peak->peakGraphic();

        graphic->showLabel(false);
        graphic->setColor(Qt::transparent);
        graphic->initFromPeakViewWidget(_peak_view_widget, peak->peak()->caughtByFilter());
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
