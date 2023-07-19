//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/subframe_filter/SubframeFilterPeaks.cpp
//! @brief     Implements class SubframeFilterPeaks
//!
//! @homepage  https://openhkl.org
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
#include "gui/graphics/PeakCollectionGraphics.h"
#include "gui/models/Meta.h"
#include "gui/models/Project.h"
#include "gui/models/Session.h"
#include "gui/utility/DataComboBox.h"
#include "gui/utility/GridFiller.h"
#include "gui/utility/PeakComboBox.h"
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
#include <QMessageBox>
#include <QRadioButton>
#include <QScrollBar>
#include <QSignalBlocker>
#include <QSpacerItem>
#include <QTableWidgetItem>

#include <sstream>

SubframeFilterPeaks::SubframeFilterPeaks()
    : QWidget()
    , _peak_collection("temp", ohkl::PeakCollectionType::FOUND, nullptr)
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
    setRejectionFlagsUp();
    setProceedUp();
    setFigureUp();
    setPeakTableUp();
    toggleUnsafeWidgets();

    _right_element->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    auto propertyScrollArea = new PropertyScrollArea(this);
    propertyScrollArea->setContentLayout(_left_layout);

    _main_layout->addWidget(propertyScrollArea);
    _main_layout->addWidget(_right_element);

    _right_element->setStretchFactor(0, 2);
    _right_element->setStretchFactor(1, 1);

    connect(
        _peak_view_widget, &PeakViewWidget::settingsChanged, _detector_widget,
        &DetectorWidget::refresh);
}

void SubframeFilterPeaks::setInputUp()
{
    auto input_box = new Spoiler("Input");
    GridFiller f(input_box, true);

    _peak_combo = f.addPeakCombo(ComboType::PeakCollection, "Peak collection");

    connect(_peak_combo, &QComboBox::currentTextChanged, this, &SubframeFilterPeaks::refreshAll);

    _left_layout->addWidget(input_box);
}

void SubframeFilterPeaks::setStateUp()
{
    _state_box = new SpoilerCheck("Type of peak");
    GridFiller f(_state_box);

    _enabled = f.addCheckBox("Valid");
    _masked = f.addCheckBox("Masked");
    _indexed_peaks = f.addCheckBox("Indexed");

    _left_layout->addWidget(_state_box);
}

void SubframeFilterPeaks::setUnitCellUp()
{
    _unit_cell_box = new SpoilerCheck("Peak indexed by given unit cell");
    GridFiller f(_unit_cell_box);

    _unit_cell = f.addCellCombo("Unit cell:");
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

    _sum_radio_1 = new QRadioButton("Sum intensities", _strength_box);
    _profile_radio_1 = new QRadioButton("Profile intensities", _strength_box);
    f.addWidget(_sum_radio_1, 1);
    f.addWidget(_profile_radio_1, 1);
    _sum_radio_1->setChecked(true);

    _strength_min = f.addDoubleSpinBox("Minimum:");
    _strength_min->setValue(1.00000);
    _strength_min->setMinimum(-1000000);
    _strength_min->setMaximum(1000000);
    _strength_min->setDecimals(2);

    _strength_max = f.addDoubleSpinBox("Maximum:");
    _strength_max->setValue(3.00000);
    _strength_max->setMinimum(1000000);
    _strength_max->setMaximum(-1000000);
    _strength_max->setDecimals(2);

    _left_layout->addWidget(_strength_box);
}

void SubframeFilterPeaks::setRangeUp()
{
    _d_range_box = new SpoilerCheck("Resolution range");
    GridFiller f(_d_range_box);

    _d_range_min = f.addDoubleSpinBox("Maximum (d min):");
    _d_range_min->setMaximum(100);
    _d_range_min->setDecimals(6);
    _d_range_min->setValue(0.0000);

    _d_range_max = f.addDoubleSpinBox("Minimum (d max):");
    _d_range_max->setMaximum(100);
    _d_range_max->setDecimals(6);
    _d_range_max->setValue(100.00000);

    _left_layout->addWidget(_d_range_box);
}

void SubframeFilterPeaks::setFrameRangeUp()
{
    _frame_range_box = new SpoilerCheck("Detector image range");
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

    _sum_radio_2 = new QRadioButton("Sum intensities", _strength_box);
    _profile_radio_2 = new QRadioButton("Profile intensities", _strength_box);
    f.addWidget(_sum_radio_2, 1);
    f.addWidget(_profile_radio_2, 1);
    _sum_radio_2->setChecked(true);

    _significance_level = f.addDoubleSpinBox("Significance:");
    _significance_level->setValue(0.990000);
    _significance_level->setMaximum(1000);
    _significance_level->setDecimals(6);

    _left_layout->addWidget(_merge_box);
}

void SubframeFilterPeaks::setOverlapUp()
{
    _overlap_box = new SpoilerCheck("Overlapping peaks");
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

void SubframeFilterPeaks::setRejectionFlagsUp()
{
    _rejection_flag_box = new SpoilerCheck("Rejection reason");
    GridFiller f(_rejection_flag_box);

    _rejection_flag_combo = f.addCombo("Rejection reason");
    for (const auto& [flag, reason] : ohkl::Peak3D::rejectionMap())
        _rejection_flag_combo->addItem(QString::fromStdString(reason));

    _left_layout->addWidget(_rejection_flag_box);
}

void SubframeFilterPeaks::setProceedUp()
{
    _extinct_spacegroup = new QCheckBox("Remove extinct from spacegroup");
    _extinct_spacegroup->setChecked(false);
    _left_layout->addWidget(_extinct_spacegroup);

    _filter_button = new QPushButton("Filter");
    _left_layout->addWidget(_filter_button);

    auto show_hide_peaks = new Spoiler("Show/hide peaks");
    _peak_view_widget = new PeakViewWidget("Peaks caught by filter", "Peaks rejected by filter");
    show_hide_peaks->setContentLayout(*_peak_view_widget);

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
    QGroupBox* figure_group = new QGroupBox("Detector image");
    figure_group->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    _detector_widget = new DetectorWidget(1, false, true, figure_group);
    _detector_widget->setVisualisationMode(VisualisationType::Filtered);
    _detector_widget->linkPeakModel(&_peak_collection_model, _peak_view_widget);

    connect(
        _detector_widget->scene(), &DetectorScene::signalUpdateDetectorScene, this,
        &SubframeFilterPeaks::refreshPeakTable);
    connect(
        _detector_widget->scene(), &DetectorScene::signalSelectedPeakItemChanged, this,
        &SubframeFilterPeaks::changeSelected);
    connect(_peak_combo, &QComboBox::currentTextChanged, this, &SubframeFilterPeaks::refreshPeakTable);
    connect(_detector_widget->dataCombo(), &QComboBox::currentTextChanged, this, &SubframeFilterPeaks::refreshPeakTable);

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

    _peak_table->setColumnHidden(PeakColumn::Count, true);

    peak_grid->addWidget(_peak_table, 0, 0, 0, 0);

    _right_element->addWidget(peak_group);
}

void SubframeFilterPeaks::refreshAll()
{
    if (!gSession->hasProject())
        return;

    _peak_combo->refresh();
    _unit_cell->refresh();
    _detector_widget->refresh();
    refreshPeakTable();
    grabFilterParameters();
    toggleUnsafeWidgets();
}

void SubframeFilterPeaks::grabFilterParameters()
{
    if (!gSession->hasProject())
        return;

    auto* params = gSession->currentProject()->experiment()->peakFilter()->parameters();

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
    _sum_radio_1->setChecked(params->sum_intensities);
    _rejection_flag_combo->setCurrentIndex(static_cast<int>(params->rejection_flag));

    auto* flags = gSession->currentProject()->experiment()->peakFilter()->flags();

    _enabled->setChecked(flags->enabled);
    _masked->setChecked(flags->masked);
    _indexed_peaks->setChecked(flags->indexed);
    _extinct_spacegroup->setChecked(flags->extinct);
    _unit_cell_box->setChecked(flags->index_tol);
    _strength_box->setChecked(flags->strength);
    _d_range_box->setChecked(flags->d_range);
    _frame_range_box->setChecked(flags->frames);
    _sparse_box->setChecked(flags->sparse);
    _merge_box->setChecked(flags->significance);
    _overlap_box->setChecked(flags->overlapping);
    _rejection_flag_box->setChecked(flags->rejection_flag);
}

void SubframeFilterPeaks::setFilterParameters()
{
    if (!gSession->hasProject())
        return;

    auto* filter = gSession->currentProject()->experiment()->peakFilter();
    auto* flags = filter->flags();
    filter->resetFilterFlags();

    if (_state_box->isChecked()) {
        flags->enabled = _enabled->isChecked();
        flags->masked = _masked->isChecked();
        flags->indexed = _indexed_peaks->isChecked();
    } else {
        flags->enabled = false;
        flags->masked = false;
        flags->indexed = false;
    }
    flags->extinct = _extinct_spacegroup->isChecked();
    flags->overlapping = _overlap_box->isChecked();
    flags->index_tol = _unit_cell_box->isChecked();
    flags->strength = _strength_box->isChecked();
    flags->d_range = _d_range_box->isChecked();
    flags->frames = _frame_range_box->isChecked();
    flags->sparse = _sparse_box->isChecked();
    flags->significance = _merge_box->isChecked();
    flags->rejection_flag = _rejection_flag_box->isChecked();

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
    params->sum_intensities = _sum_radio_1->isChecked();
    params->rejection_flag =
        static_cast<ohkl::RejectionFlag>(_rejection_flag_combo->currentIndex());
}

void SubframeFilterPeaks::filterPeaks()
{
    gGui->setReady(false);
    ohkl::PeakFilter* filter = gSession->currentProject()->experiment()->peakFilter();
    ohkl::PeakCollection* collection = _peak_combo->currentPeakCollection();
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
    ohkl::PeakCollection* collection = _peak_combo->currentPeakCollection();
    ohkl::sptrDataSet data = collection->data();

    std::string suggestion = gSession->currentProject()->experiment()->generatePeakCollectionName();
    std::unique_ptr<ListNameDialog> dlg(new ListNameDialog(QString::fromStdString(suggestion)));
    dlg->exec();
    if (dlg->listName().isEmpty())
        return;
    if (dlg->result() == QDialog::Rejected)
        return;
    if (!gSession->currentProject()->experiment()->acceptFilter(
            dlg->listName().toStdString(), collection, collection->type(), data)) {
        QMessageBox::warning(
            this, "Unable to add PeakCollection", "Collection with this name already exists!");
        return;
    }

    gSession->onPeaksChanged();
    _peak_combo->refresh();
    gSession->currentProject()->generatePeakModel(dlg->listName());
}

void SubframeFilterPeaks::refreshPeakTable()
{
    if (!gSession->hasProject())
        return;
    if (!gSession->currentProject()->hasPeakCollection())
        return;

    ohkl::PeakCollection* collection = _peak_combo->currentPeakCollection();

    if (!collection) // if no PeakCollection has been selected from the GUI
        return;

    _peak_collection_item.setPeakCollection(collection);
    _peak_collection_item.setFilterMode();
    _peak_collection_model.setRoot(&_peak_collection_item);
    _peak_table->resizeColumnsToContents();

    _peak_table->setColumnHidden(PeakColumn::Count, true);
    _peak_table->setColumnHidden(PeakColumn::BkgGradient, true);
    _peak_table->setColumnHidden(PeakColumn::BkgGradientSigma, true);

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
    _filter_button->setEnabled(false);
    _save_button->setEnabled(false);

    if (!gSession->hasProject())
        return;

    _filter_button->setEnabled(gSession->currentProject()->hasPeakCollection());
    _save_button->setEnabled(gSession->currentProject()->hasPeakCollection());
}

DetectorWidget* SubframeFilterPeaks::detectorWidget()
{
    return _detector_widget;
}
