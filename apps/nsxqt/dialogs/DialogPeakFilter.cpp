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
#include "MetaTypes.h"
#include "DialogPeakFilter.h"

DialogPeakFilter::DialogPeakFilter(const nsx::PeakList& peaks, QWidget* parent)
: QDialog(parent),
  _ui(new Ui::DialogPeakFilter),
  _peaks(peaks),
  _filtered_peaks()
{
    _ui->setupUi(this);

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

    // Populate the unit cell combobox
    std::set<nsx::sptrUnitCell> unit_cells;
    for (auto peak : peaks) {
        auto unit_cell = peak->unitCell();
        if (!unit_cell) {
            continue;
        }
        auto it = unit_cells.find(unit_cell);
        if (it == unit_cells.end()) {
            unit_cells.insert(unit_cell);
        }
    }
    for (auto unit_cell : unit_cells) {

        _ui->unitCells->addItem(QString::fromStdString(unit_cell->name()),QVariant::fromValue(unit_cell));
    }

    connect(_ui->unitCells,SIGNAL(currentIndexChanged(int)),this,SLOT(slotUnitCellChanged(int)));

    connect(_ui->actions,SIGNAL(clicked(QAbstractButton*)),this,SLOT(slotActionClicked(QAbstractButton*)));
    connect(_ui->actions,SIGNAL(accepted()), this, SLOT(accept()));
    connect(_ui->actions,SIGNAL(rejected()), this, SLOT(reject()));
}

DialogPeakFilter::~DialogPeakFilter()
{
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

void DialogPeakFilter::filterPeaks() {

    _filtered_peaks.clear();

    _filtered_peaks = _peaks;

    nsx::PeakFilter peak_filter;

    if (_ui->state->isChecked()) {
        _filtered_peaks = peak_filter.selected(_filtered_peaks,_ui->selected->isChecked());
        _filtered_peaks = peak_filter.masked(_filtered_peaks,_ui->masked->isChecked());
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
        double dmin = _ui->dmin->value();
        double dmax = _ui->dmax->value();

        _filtered_peaks = peak_filter.dRange(_filtered_peaks,dmin,dmax);
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
}

void DialogPeakFilter::accept()
{
    QDialog::accept();
}

const nsx::PeakList& DialogPeakFilter::filteredPeaks() const
{
    return _filtered_peaks;
}
