#include <QAbstractButton>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QDoubleSpinBox>
#include <QGroupBox>
#include <QLabel>
#include <QVBoxLayout>

#include <nsxlib/DataSet.h>
#include <nsxlib/Logger.h>
#include <nsxlib/Peak3D.h>
#include <nsxlib/PeakFilter.h>
#include <nsxlib/UnitCell.h>

#include "ui_DialogPeakFilter.h"
#include "CollectedPeaksModel.h"
#include "DialogPeakFilter.h"
#include "ExperimentItem.h"
#include "PeaksItem.h"
#include "MetaTypes.h"

DialogPeakFilter* DialogPeakFilter::_instance = nullptr;

DialogPeakFilter* DialogPeakFilter::create(ExperimentItem* experiment_item, const nsx::PeakList& peaks, QWidget* parent)
{
    if (!_instance) {
        _instance = new DialogPeakFilter(experiment_item, peaks, parent);
    }

    return _instance;
}

DialogPeakFilter* DialogPeakFilter::Instance()
{
    return _instance;
}

const nsx::PeakList& DialogPeakFilter::peaks() const
{
    return _peaks;
}

DialogPeakFilter::DialogPeakFilter(ExperimentItem* experiment_item, const nsx::PeakList& peaks, QWidget* parent)
: QDialog(parent),
  _ui(new Ui::DialogPeakFilter),
  _experiment_item(experiment_item),
  _peaks(peaks),
  _filtered_peaks()
{
    _ui->setupUi(this);

    setModal(false);
    setWindowModality(Qt::NonModal);

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
    double dmin(std::numeric_limits<double>::infinity()), dmax(-std::numeric_limits<double>::infinity());
    std::set<nsx::sptrUnitCell> unit_cells;
    for (auto peak : peaks) {
        auto d = 1.0/peak->q().rowVector().norm();
        dmin = std::min(dmin,d);
        dmax = std::max(dmax,d);
        auto unit_cell = peak->unitCell();
        if (!unit_cell) {
            continue;
        }
        auto it = unit_cells.find(unit_cell);
        if (it == unit_cells.end()) {
            unit_cells.insert(unit_cell);
        }
    }

    _ui->dMin->setValue(dmin);
    _ui->dMax->setValue(dmax);

    for (auto unit_cell : unit_cells) {
        _ui->unitCells->addItem(QString::fromStdString(unit_cell->name()),QVariant::fromValue(unit_cell));
    }

    _peaks_model = new CollectedPeaksModel(_experiment_item->model(),_experiment_item->experiment(),_peaks);
    _ui->peaks->setModel(_peaks_model);

    connect(_ui->unitCells,SIGNAL(currentIndexChanged(int)),this,SLOT(slotUnitCellChanged(int)));

    connect(_ui->actions,SIGNAL(clicked(QAbstractButton*)),this,SLOT(slotActionClicked(QAbstractButton*)));
}

DialogPeakFilter::~DialogPeakFilter()
{
    if (_peaks_model) {
        delete _peaks_model;
    }

    if (_instance) {
        delete _instance;
        _instance = nullptr;
    }

    delete _ui;
}

void DialogPeakFilter::slotUnitCellChanged(int index) {

    auto unit_cell = _ui->unitCells->itemData(index,Qt::UserRole).value<nsx::sptrUnitCell>();

    _ui->indexingTolerance->setValue(unit_cell->indexingTolerance());
}

void DialogPeakFilter::slotActionClicked(QAbstractButton *button)
{
    auto button_role = _ui->actions->standardButton(button);

    switch(button_role)
    {
    case QDialogButtonBox::StandardButton::Apply: {
        filterPeaks();
        break;
    }
    default: {
        return;
    }
    }
}

void DialogPeakFilter::filterPeaks()
{
    _filtered_peaks.clear();

    _filtered_peaks = _peaks;

    nsx::PeakFilter peak_filter;

    if (_ui->state->isChecked()) {
        _filtered_peaks = peak_filter.selected(_filtered_peaks,_ui->selected->isChecked());
        _filtered_peaks = peak_filter.masked(_filtered_peaks,_ui->masked->isChecked());
        _filtered_peaks = peak_filter.predicted(_filtered_peaks,_ui->predicted->isChecked());
    }

    if (_ui->indexed->isChecked()) {
        _filtered_peaks = peak_filter.indexed(_filtered_peaks);
    }

    if (_ui->indexedByUnitCell->isChecked()) {
        if (_ui->unitCells->count() > 0) {
            auto unit_cell = _ui->unitCells->itemData(_ui->unitCells->currentIndex(),Qt::UserRole).value<nsx::sptrUnitCell>();
            _filtered_peaks = peak_filter.indexed(_filtered_peaks,unit_cell,_ui->indexingTolerance->value());
        }
    }

    if (_ui->strength->isChecked()) {
        double smin = _ui->strengthMin->value();
        double smax = _ui->strengthMax->value();

        _filtered_peaks = peak_filter.strength(_filtered_peaks,smin,smax);
    }

    if (_ui->dRange->isChecked()) {
        double d_min = _ui->dMin->value();
        double d_max = _ui->dMax->value();

        _filtered_peaks = peak_filter.dRange(_filtered_peaks,d_min,d_max);
    }

    if (_ui->extincted->isChecked()) {
        _filtered_peaks = peak_filter.extincted(_filtered_peaks);
    }

    if (_ui->sparseDataSets->isChecked()) {
        size_t min_num_peaks = static_cast<size_t>(_ui->minNumPeaks->value());

        _filtered_peaks = peak_filter.sparseDataSet(_filtered_peaks,min_num_peaks);
    }

    if (_ui->mergedPeakSignificance->isChecked()) {
        double significance_level = _ui->significanceLevel->value();

        _filtered_peaks = peak_filter.mergedPeaksSignificance(_filtered_peaks,significance_level);
    }

    if (_ui->overlapping->isChecked()) {
        _filtered_peaks = peak_filter.overlapping(_filtered_peaks);
    }

    if (_ui->complementary->isChecked()) {
        _filtered_peaks = peak_filter.complementary(_peaks,_filtered_peaks);
    }

    if (_peaks_model) {
        delete _peaks_model;
        QItemSelectionModel *selection_model = _ui->peaks->selectionModel();
        delete selection_model;
    }
    _peaks_model = new CollectedPeaksModel(_experiment_item->model(),_experiment_item->experiment(),_filtered_peaks);

    _ui->peaks->setModel(_peaks_model);

}

void DialogPeakFilter::accept()
{
    if (!_filtered_peaks.empty()) {
        _experiment_item->peaksItem()->filterPeaks(_peaks,_filtered_peaks);
    }

    QDialog::accept();
}

const nsx::PeakList& DialogPeakFilter::filteredPeaks() const
{
    return _filtered_peaks;
}
