//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      apps/dialogs/DialogPeakFilter.cpp
//! @brief     Implements class DialogPeakFilter
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include <QAbstractButton>
#include <QComboBox>
#include <QDebug>
#include <QDialogButtonBox>
#include <QDoubleSpinBox>
#include <QGroupBox>
#include <QLabel>
#include <QVBoxLayout>
#include <QtGlobal>

#include "core/analyse/PeakFilter.h"
#include "core/experiment/DataSet.h"
#include "core/peak/Peak3D.h"
#include "tables/crystal/UnitCell.h"

#include "apps/dialogs/DialogPeakFilter.h"
#include "apps/models/CollectedPeaksModel.h"
#include "apps/models/ExperimentItem.h"
#include "apps/models/MetaTypes.h"
#include "apps/models/PeakListItem.h"
#include "apps/models/PeaksItem.h"
#include "ui_DialogPeakFilter.h"

DialogPeakFilter* DialogPeakFilter::_instance = nullptr;

DialogPeakFilter* DialogPeakFilter::create(
    ExperimentItem* experiment_item, const nsx::PeakList& peaks, QWidget* parent)
{
    if (!_instance)
        _instance = new DialogPeakFilter(experiment_item, peaks, parent);

    return _instance;
}

DialogPeakFilter* DialogPeakFilter::Instance()
{
    return _instance;
}

DialogPeakFilter::DialogPeakFilter(
    ExperimentItem* experiment_item, const nsx::PeakList& peaks, QWidget* parent)
    : QDialog(parent)
    , _ui(new Ui::DialogPeakFilter)
    , _experiment_item(experiment_item)
    , _peaks(peaks)
{
    _ui->setupUi(this);

    setModal(false);

    setWindowModality(Qt::NonModal);

    setAttribute(Qt::WA_DeleteOnClose);

    _ui->state->setStyleSheet("font-weight: normal;");
    _ui->state->setCheckable(true);
    _ui->state->setChecked(true);

    _ui->indexedByUnitCell->setStyleSheet("font-weight: normal;");
    _ui->indexedByUnitCell->setCheckable(true);
    _ui->indexedByUnitCell->setChecked(false);

    _ui->strength->setStyleSheet("font-weight: normal;");
    _ui->strength->setCheckable(true);
    _ui->strength->setChecked(false);
    _ui->strengthMax->setValue(std::numeric_limits<double>::infinity());

    _ui->sparseDataSets->setStyleSheet("font-weight: normal;");
    _ui->sparseDataSets->setCheckable(true);
    _ui->sparseDataSets->setChecked(false);

    _ui->mergedPeakSignificance->setStyleSheet("font-weight: normal;");
    _ui->mergedPeakSignificance->setCheckable(true);
    _ui->mergedPeakSignificance->setChecked(false);

    _ui->dRange->setStyleSheet("font-weight: normal;");
    _ui->dRange->setCheckable(true);
    _ui->dRange->setChecked(false);

    // Populate the unit cell combobox and set the dmin and dmax value
    double dmin(std::numeric_limits<double>::infinity()),
        dmax(-std::numeric_limits<double>::infinity());
    std::set<nsx::sptrUnitCell> unit_cells;
    for (auto peak : peaks) {
        auto d = 1.0 / peak->q().rowVector().norm();
        dmin = std::min(dmin, d);
        dmax = std::max(dmax, d);
        auto unit_cell = peak->unitCell();
        if (!unit_cell)
            continue;
        auto it = unit_cells.find(unit_cell);
        if (it == unit_cells.end())
            unit_cells.insert(unit_cell);
    }

    _ui->dMin->setValue(dmin);
    _ui->dMax->setValue(dmax);

    for (auto unit_cell : unit_cells) {
        _ui->unitCells->addItem(
            QString::fromStdString(unit_cell->name()), QVariant::fromValue(unit_cell));
    }

    _peaks_model =
        new CollectedPeaksModel(_experiment_item->model(), _experiment_item->experiment(), _peaks);
    _ui->peaks->setModel(_peaks_model);

    connect(_ui->unitCells, SIGNAL(currentIndexChanged(int)), this, SLOT(slotUnitCellChanged(int)));

    connect(
        _ui->actions, SIGNAL(clicked(QAbstractButton*)), this,
        SLOT(slotActionClicked(QAbstractButton*)));
}

DialogPeakFilter::~DialogPeakFilter()
{
    if (_peaks_model)
        delete _peaks_model;

    delete _ui;

    if (_instance)
        _instance = nullptr;
}

void DialogPeakFilter::slotUnitCellChanged(int index)
{
    auto unit_cell = _ui->unitCells->itemData(index, Qt::UserRole).value<nsx::sptrUnitCell>();

    _ui->indexingTolerance->setValue(unit_cell->indexingTolerance());
}

void DialogPeakFilter::slotActionClicked(QAbstractButton* button)
{
    auto button_role = _ui->actions->standardButton(button);

    switch (button_role) {
        case QDialogButtonBox::StandardButton::Apply: {
            filterPeaks();
            break;
        }
        case QDialogButtonBox::StandardButton::Cancel: {
            reject();
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

void DialogPeakFilter::filterPeaks()
{
    nsx::PeakList filtered_peaks = _peaks;

    nsx::PeakFilter peak_filter;

    if (_ui->state->isChecked()) {
        filtered_peaks = peak_filter.selected(filtered_peaks, _ui->selected->isChecked());
        filtered_peaks = peak_filter.masked(filtered_peaks, _ui->masked->isChecked());
        filtered_peaks = peak_filter.predicted(filtered_peaks, _ui->predicted->isChecked());
    }

    if (_ui->indexed->isChecked())
        filtered_peaks = peak_filter.indexed(filtered_peaks);

    if (_ui->indexedByUnitCell->isChecked()) {
        if (_ui->unitCells->count() > 0) {
            auto unit_cell = _ui->unitCells->itemData(_ui->unitCells->currentIndex(), Qt::UserRole)
                                 .value<nsx::sptrUnitCell>();
            filtered_peaks =
                peak_filter.indexed(filtered_peaks, *unit_cell, _ui->indexingTolerance->value());
        }
    }

    if (_ui->strength->isChecked()) {
        double smin = _ui->strengthMin->value();
        double smax = _ui->strengthMax->value();

        filtered_peaks = peak_filter.strength(filtered_peaks, smin, smax);
    }

    if (_ui->dRange->isChecked()) {
        double d_min = _ui->dMin->value();
        double d_max = _ui->dMax->value();

        filtered_peaks = peak_filter.dRange(filtered_peaks, d_min, d_max);
    }

    if (_ui->extincted->isChecked())
        filtered_peaks = peak_filter.extincted(filtered_peaks);

    if (_ui->sparseDataSets->isChecked()) {
        size_t min_num_peaks = static_cast<size_t>(_ui->minNumPeaks->value());

        filtered_peaks = peak_filter.sparseDataSet(filtered_peaks, min_num_peaks);
    }

    if (_ui->mergedPeakSignificance->isChecked()) {
        double significance_level = _ui->significanceLevel->value();

        filtered_peaks = peak_filter.mergedPeaksSignificance(filtered_peaks, significance_level);
    }

    if (_ui->overlapping->isChecked())
        filtered_peaks = peak_filter.overlapping(filtered_peaks);

    if (_ui->complementary->isChecked())
        filtered_peaks = peak_filter.complementary(_peaks, filtered_peaks);

    _peaks_model->setPeaks(filtered_peaks);
}

void DialogPeakFilter::accept()
{
    auto& filtered_peaks = _peaks_model->peaks();

    if (!filtered_peaks.empty()) {
        auto peak_list = new PeakListItem(filtered_peaks);

        peak_list->setText("Filtered peaks");

        _experiment_item->peaksItem()->appendRow(peak_list);

        qInfo() << "Applied peak filters on selected peaks. Remains " << filtered_peaks.size()
                << " out of " << _peaks.size() << " peaks";
    }

    QDialog::accept();
}
