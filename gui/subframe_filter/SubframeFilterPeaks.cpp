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
#include "gui/MainWin.h" // gGui
#include "gui/dialogs/ListNameDialog.h"
#include "gui/graphics/DetectorScene.h"
#include "gui/models/Meta.h"
#include "gui/models/Project.h"
#include "gui/models/Session.h"
#include "gui/utility/GridFiller.h"
#include "gui/utility/LinkedComboBox.h"
#include "gui/utility/PropertyScrollArea.h"
#include "gui/utility/SafeSpinBox.h"
#include "gui/utility/SideBar.h"
#include "gui/utility/Spoiler.h"
#include "gui/utility/SpoilerCheck.h"
#include "gui/widgets/DetectorWidget.h"
#include "gui/widgets/PeakViewWidget.h"

#include <QFileInfo>
#include <QGridLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QItemDelegate>
#include <QLabel>
#include <QScrollBar>
#include <QSpacerItem>
#include <QTableWidgetItem>

#include <sstream>

SubframeFilterPeaks::SubframeFilterPeaks()
    : QWidget()
    , _peak_collection("temp", nsx::listtype::FOUND)
    , _peak_collection_item()
    , _peak_collection_model()
{
    _main_layout = new QHBoxLayout(this);
    _right_element = new QSplitter(Qt::Vertical, this);

    _left_layout = new QVBoxLayout();

    setInputUp();
    setStateUp();
    setUnitCellUp();
    setStrengthUp();
    setRangeUp();
    setFrameRangeUp();
    setSparseUp();
    setMergeUp();
    setOverlapUp();
    setProceedUp();
    setFigureUp();
    setPeakTableUp();

    _right_element->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    auto propertyScrollArea = new PropertyScrollArea(this);
    propertyScrollArea->setContentLayout(_left_layout);

    _main_layout->addWidget(propertyScrollArea);
    _main_layout->addWidget(_right_element);
}

void SubframeFilterPeaks::setInputUp()
{
    auto input_box = new Spoiler("Input");
    GridFiller f(input_box, true);

    _exp_combo = f.addLinkedCombo(ComboType::Experiment, "Experiment");
    _peak_combo = f.addLinkedCombo(ComboType::PeakCollection, "Peak collection");

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

    _unit_cell = f.addLinkedCombo(ComboType::UnitCell, "Unit cell:");
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
    _d_range_min->setMaximum(100);
    _d_range_min->setDecimals(6);
    _d_range_min->setValue(0.0000);

    _d_range_max = f.addDoubleSpinBox("Maximum:");
    _d_range_max->setMaximum(100);
    _d_range_max->setDecimals(6);
    _d_range_max->setValue(100.00000);

    _left_layout->addWidget(_d_range_box);
}

void SubframeFilterPeaks::setFrameRangeUp()
{
    _frame_range_box = new SpoilerCheck("Frame range");
    GridFiller f(_frame_range_box);

    _frame_min = f.addDoubleSpinBox("Minimum:");
    _frame_min->setMaximum(1000);
    _frame_min->setDecimals(0);
    _frame_min->setValue(0.0000);

    _frame_max = f.addDoubleSpinBox("Maximum:");
    _frame_max->setMaximum(1000);
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
    _min_number_peaks->setMaximum(10000);

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

void SubframeFilterPeaks::setOverlapUp()
{
    _overlap_box = new SpoilerCheck("Remove overlapping peaks");
    GridFiller f(_overlap_box);

    _peak_end = f.addDoubleSpinBox("Peak end");
    _peak_end->setValue(3.0);
    _peak_end->setMinimum(1.0);
    _peak_end->setMaximum(10.0);
    _peak_end->setDecimals(2);

    _bkg_end = f.addDoubleSpinBox("Background end");
    _bkg_end->setValue(6.0);
    _bkg_end->setMaximum(10.0);
    _bkg_end->setMinimum(0.0);
    _bkg_end->setDecimals(2);

    _left_layout->addWidget(_overlap_box);
}

void SubframeFilterPeaks::setProceedUp()
{
    _extinct_spacegroup = new QCheckBox("Remove extinct from spacegroup");
    _extinct_spacegroup->setChecked(false);
    _left_layout->addWidget(_extinct_spacegroup);

    _keep_complementary = new QCheckBox("Keep the complementary selection");
    _keep_complementary->setChecked(false);
    _left_layout->addWidget(_keep_complementary);

    _filter_button = new QPushButton("Filter");
    _left_layout->addWidget(_filter_button);

    auto show_hide_peaks = new Spoiler("Show/hide peaks");
    _peak_view_widget = new PeakViewWidget("Peaks caught by filter", "Peaks rejected by filter");
    show_hide_peaks->setContentLayout(*_peak_view_widget);

    connect(
        _peak_view_widget, &PeakViewWidget::settingsChanged, this,
        &SubframeFilterPeaks::refreshPeakVisual);
    connect(
        _peak_view_widget->set1.peakEnd, qOverload<double>(&QDoubleSpinBox::valueChanged),
        _peak_end, &QDoubleSpinBox::setValue);
    connect(
        _peak_view_widget->set1.bkgEnd, qOverload<double>(&QDoubleSpinBox::valueChanged), _bkg_end,
        &QDoubleSpinBox::setValue);
    connect(
        _peak_end, qOverload<double>(&QDoubleSpinBox::valueChanged),
        _peak_view_widget->set1.peakEnd, &QDoubleSpinBox::setValue);
    connect(
        _bkg_end, qOverload<double>(&QDoubleSpinBox::valueChanged), _peak_view_widget->set1.bkgEnd,
        &QDoubleSpinBox::setValue);

    _left_layout->addWidget(show_hide_peaks);

    _save_button = new QPushButton("Create peak collection");
    _left_layout->addWidget(_save_button);

    connect(_filter_button, &QPushButton::clicked, this, &SubframeFilterPeaks::filterPeaks);
    connect(_save_button, &QPushButton::clicked, this, &SubframeFilterPeaks::accept);
    connect(
        gGui->sideBar(), &SideBar::subframeChanged, this,
        &SubframeFilterPeaks::setFilterParameters);
}

void SubframeFilterPeaks::setFigureUp()
{
    QGroupBox* figure_group = new QGroupBox("Preview");
    figure_group->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    _detector_widget = new DetectorWidget(false, false, true, figure_group);
    _detector_widget->linkPeakModel(&_peak_collection_model);

    connect(
        _detector_widget->scene(), &DetectorScene::signalUpdateDetectorScene,
        this, &SubframeFilterPeaks::refreshPeakTable);
    connect(
        _detector_widget->scene(), &DetectorScene::signalSelectedPeakItemChanged, this,
        &SubframeFilterPeaks::changeSelected);
    connect(
        _peak_combo, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this,
        &SubframeFilterPeaks::refreshPeakTable);

    _right_element->addWidget(figure_group);
}

void SubframeFilterPeaks::setPeakTableUp()
{
    QGroupBox* peak_group = new QGroupBox("Peaks");
    QGridLayout* peak_grid = new QGridLayout(peak_group);

    peak_group->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    _peak_table = new PeakTableView(this);
    _peak_collection_model.setRoot(&_peak_collection_item);
    _peak_table->setModel(&_peak_collection_model);
    _peak_table->resizeColumnsToContents();

    peak_grid->addWidget(_peak_table, 0, 0, 0, 0);

    _right_element->addWidget(peak_group);
}

void SubframeFilterPeaks::refreshAll()
{
    setParametersUp();
    toggleUnsafeWidgets();
}

void SubframeFilterPeaks::setParametersUp()
{
    setExperimentsUp();
}

void SubframeFilterPeaks::setExperimentsUp()
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
    updatePeakList();
    grabFilterParameters();
}

void SubframeFilterPeaks::updatePeakList()
{
    if (gSession->experimentNames().empty())
        return;

    QString current_peaks = _peak_combo->currentText();
    _peak_combo->clear();
    auto peak_list = gSession->experimentAt(_exp_combo->currentIndex())->getPeakListNames();
    _peak_combo->addItems(peak_list);
    _peak_combo->setCurrentText(current_peaks);

    updateDatasetList();
    refreshPeakTable();
}

void SubframeFilterPeaks::updateDatasetList()
{
    _data_list = gSession->experimentAt(_exp_combo->currentIndex())->allData();
    const nsx::DataList all_data = gSession->experimentAt(_exp_combo->currentIndex())->allData();
    if (!_data_list.empty())
        _detector_widget->updateDatasetList(all_data);
}

void SubframeFilterPeaks::grabFilterParameters()
{
    if (_peak_combo->count() == 0 || _exp_combo->count() == 0)
        return;

    auto* params =
        gSession->experimentAt(_exp_combo->currentIndex())->experiment()->peakFilter()->parameters();

    _tolerance->setValue(params->unit_cell_tolerance);
    _strength_min->setValue(params->strength_min);
    _strength_max->setValue(params->strength_max);
    _d_range_min->setValue(params->d_min);
    _d_range_max->setValue(params->d_max);
    _frame_min->setValue(params->frame_min);
    _frame_max->setValue(params->frame_max);
    _significance_level->setValue(params->significance);
    _peak_end->setValue(params->peak_end);
    _bkg_end->setValue(params->bkg_end);

    auto* flags =
        gSession->experimentAt(_exp_combo->currentIndex())->experiment()->peakFilter()->flags();

    _selected->setChecked(flags->selected);
    _masked->setChecked(flags->masked);
    _predicted->setChecked(flags->predicted);
    _indexed_peaks->setChecked(flags->indexed);
    _extinct_spacegroup->setChecked(flags->extinct);
    _keep_complementary->setChecked(flags->complementary);
    _state_box->setChecked(flags->state);
    _unit_cell_box->setChecked(flags->index_tol);
    _strength_box->setChecked(flags->strength);
    _d_range_box->setChecked(flags->d_range);
    _frame_range_box->setChecked(flags->frames);
    _sparse_box->setChecked(flags->sparse);
    _merge_box->setChecked(flags->significance);
    _overlap_box->setChecked(flags->overlapping);
}

void SubframeFilterPeaks::setFilterParameters()
{
    if (_peak_combo->count() == 0 || _exp_combo->count() == 0)
        return;

    auto* filter = gSession->experimentAt(_exp_combo->currentIndex())->experiment()->peakFilter();
    auto* flags = filter->flags();
    filter->resetFilterFlags();

    if (_selected->isChecked())
        flags->selected = true;
    if (_masked->isChecked())
        flags->masked = true;
    if (_predicted->isChecked())
        flags->predicted = true;
    if (_indexed_peaks->isChecked())
        flags->indexed = true;
    if (_extinct_spacegroup->isChecked())
        flags->extinct = true;
    if (_overlap_box->isChecked())
        flags->overlapping = true;
    if (_keep_complementary->isChecked())
        flags->complementary = true;
    if (_state_box->isChecked())
        flags->state = true;
    if (_unit_cell_box->isChecked())
        flags->index_tol = true;
    if (_strength_box->isChecked())
        flags->strength = true;
    if (_d_range_box->isChecked())
        flags->d_range = true;
    if (_frame_range_box->isChecked())
        flags->frames = true;
    if (_sparse_box->isChecked())
        flags->sparse = true;
    if (_merge_box->isChecked())
        flags->significance = true;

    auto* params = filter->parameters();

    params->unit_cell_tolerance = _tolerance->value();
    params->significance = _significance_level->value();
    params->d_min = _d_range_min->value();
    params->d_max = _d_range_max->value();
    params->frame_min = _frame_min->value();
    params->frame_max = _frame_max->value();
    params->strength_min = _strength_min->value();
    params->strength_max = _strength_max->value();
    params->unit_cell = _unit_cell->currentText().toStdString();
    params->peak_end = _peak_end->value();
    params->bkg_end = _bkg_end->value();
}

void SubframeFilterPeaks::filterPeaks()
{
    gGui->setReady(false);
    nsx::PeakFilter* filter =
        gSession->experimentAt(_exp_combo->currentIndex())->experiment()->peakFilter();
    nsx::PeakCollection* collection =
        gSession->experimentAt(_exp_combo->currentIndex())
            ->experiment()
            ->getPeakCollection(_peak_combo->currentText().toStdString());
    filter->resetFiltering(collection);
    setFilterParameters();
    filter->filter(collection);

    refreshPeakTable();

    int n_peaks = _peak_collection_item.numberOfPeaks();
    int n_caught = _peak_collection_item.numberCaughtByFilter();

    gGui->statusBar()->showMessage(
        QString::number(n_caught) + "/" + QString::number(n_peaks) + " caught by filter");
    gGui->setReady(true);
}

void SubframeFilterPeaks::accept()
{
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
        auto peak_list = gSession->experimentAt(_exp_combo->currentIndex())->getPeakListNames();
        _peak_combo->updateList(peak_list);
    }
}

void SubframeFilterPeaks::refreshPeakTable()
{
    if (_peak_combo->count() == 0 || _exp_combo->count() == 0)
        return;

    nsx::PeakCollection* collection =
        gSession->experimentAt(_exp_combo->currentIndex())
            ->experiment()
            ->getPeakCollection(_peak_combo->currentText().toStdString());
    _peak_collection_item.setPeakCollection(collection);
    _peak_collection_item.setFilterMode();
    _peak_collection_model.setRoot(&_peak_collection_item);
    _peak_table->resizeColumnsToContents();

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
        graphic->initFromPeakViewWidget(
            peak->peak()->caughtByFilter() ? _peak_view_widget->set1 : _peak_view_widget->set2);
    }
    _detector_widget->scene()->initIntRegionFromPeakWidget(_peak_view_widget->set1);
    _detector_widget->refresh();
}

void SubframeFilterPeaks::changeSelected(PeakItemGraphic* peak_graphic)
{
    int row = _peak_collection_item.returnRowOfVisualItem(peak_graphic);
    QModelIndex index = _peak_collection_model.index(row, 0);
    _peak_table->selectRow(row);
    _peak_table->scrollTo(index, QAbstractItemView::PositionAtTop);
}

void SubframeFilterPeaks::toggleUnsafeWidgets()
{
    _filter_button->setEnabled(true);
    _save_button->setEnabled(true);
    if (_exp_combo->count() == 0 || _peak_combo->count() == 0) {
        _filter_button->setEnabled(false);
        _save_button->setEnabled(false);
    }
}

DetectorWidget* SubframeFilterPeaks::detectorWidget()
{
    return _detector_widget;
}
