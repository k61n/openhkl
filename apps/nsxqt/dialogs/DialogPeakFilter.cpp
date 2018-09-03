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

DialogPeakFilter::DialogPeakFilter(const nsx::PeakList& peaks, QWidget* parent):
    QDialog(parent),
    _ui(new Ui::DialogPeakFilter),
    _peaks(peaks),
    _badPeaks(),
    _goodPeaks()
{
    _ui->setupUi(this);

    // Populate the unit cell combobox
    std::set<nsx::sptrUnitCell> unit_cells;
    for (auto peak : peaks) {
        auto unit_cell = peak->unitCell();
        if (!unit_cell) {
            continue;
        }
        auto it = unit_cells.find(unit_cell);
        if (it != unit_cells.end()) {
            unit_cells.insert(unit_cell);
        }
    }
    for (auto unit_cell : unit_cells) {
        _ui->unitCells->addItem(QString::fromStdString(unit_cell->name()),QVariant::fromValue(unit_cell));
    }

    connect(_ui->unitCells,SIGNAL(currentIndexChanged(int index)),this,SLOT(slotUnitCellChanged(int index)));

    connect(_ui->actions, SIGNAL(clicked(QAbstractButton*)),this,SLOT(actionClicked(QAbstractButton*)));
    connect(_ui->actions, SIGNAL(accepted()), this, SLOT(accept()));
    connect(_ui->actions, SIGNAL(rejected()), this, SLOT(reject()));
}

DialogPeakFilter::~DialogPeakFilter()
{
    delete _ui;
}

void DialogPeakFilter::slotUnitCellChanged(int index) {

    auto unit_cell = _ui->unitCells->itemData(index,Qt::UserRole).value<nsx::sptrUnitCell>();

    _ui->indexingTolerance->setValue(unit_cell->indexingTolerance());
}

void DialogPeakFilter::actionClicked(QAbstractButton *button)
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

    if (_ui->unindexed->isChecked()) {

        auto unit_cell = _ui->unitCells->itemData(_ui->unitCells->currentIndex(),Qt::UserRole).value<nsx::sptrUnitCell>();

        _filtered_peaks = peak_filter.indexed(_filtered_peaks,unit_cell,_ui->indexingTolerance->value());
    }

    if (_ui->disabled->isChecked()) {
        _filtered_peaks = peak_filter.enabled(_filtered_peaks,true);
    }

    if (_ui->dRange->isCheckable()) {

        double dmin = _ui->dmin->value();
        double dmax = _ui->dmax->value();

        _filtered_peaks = peak_filter.dRange(_filtered_peaks,dmin,dmax);

    }

    if (_ui->overlapping->isChecked()) {
        _filtered_peaks = peak_filter.overlapping(_filtered_peaks);
    }

    if (_ui->extincted->isChecked()) {
        _filtered_peaks = peak_filter.extincted();
    }

}

void DialogPeakFilter::accept()
{
    nsx::PeakFilter filter;

//    filter._removeUnselected = _ui->checkUnselected->isChecked();
//    filter._removeIsigma = _ui->checkIsigma->isChecked();
//    filter._removePValue = _ui->checkPValue->isChecked();
//    filter._removeOverlapping = _ui->checkOverlapping->isChecked();
//    filter._removeDmin = _ui->checkDmin->isChecked();
//    filter._removeDmax = _ui->checkDmax->isChecked();
//    filter._removeForbidden = _ui->checkSpaceGroup->isChecked();
//    filter._removeMergedP = _ui->checkBoxMergedP->isChecked();
//
//    filter._Isigma = _ui->spinBoxIsigma->value();
//    filter._dmin = _ui->spinBoxDmin->value();
//    filter._dmax = _ui->spinBoxDmax->value();
//    filter._pvalue = _ui->spinBoxPValue->value();
//    filter._mergedP = _ui->spinBoxMergedP->value();
//
//    nsx::info() << "Filtering peaks...";
//    _goodPeaks = filter.apply(_peaks);
//    nsx::info() << _goodPeaks.size() << " peaks remain";
//
//    _badPeaks.clear();
//
//    for (auto peak: _peaks) {
//        auto it = std::find(_goodPeaks.begin(), _goodPeaks.end(), peak);
//        if (it == _goodPeaks.end()) {
//            _badPeaks.push_back(peak);
//        }
//    }

    QDialog::accept();
}

const nsx::PeakList& DialogPeakFilter::badPeaks() const
{
    return _badPeaks;
}

const nsx::PeakList& DialogPeakFilter::goodPeaks() const
{
    return _goodPeaks;
}
