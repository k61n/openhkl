#include <QDialogButtonBox>
#include <QDoubleSpinBox>
#include <QGroupBox>
#include <QLabel>
#include <QVBoxLayout>

#include <nsxlib/data/DataSet.h>
#include <nsxlib/crystal/Peak3D.h>
#include <nsxlib/crystal/PeakFilter.h>
#include <nsxlib/logger/Logger.h>

#include "ui_PeakFilterDialog.h"
#include "DialogPeakFilter.h"



DialogPeakFilter::DialogPeakFilter(const nsx::PeakSet& peaks, QWidget* parent):
    QDialog(parent),
    _ui(new Ui::PeakFilterDialog),
    _peaks(peaks)
{
    _ui->setupUi(this);
}

DialogPeakFilter::~DialogPeakFilter()
{
    delete _ui;
}

void DialogPeakFilter::accept()
{
    nsx::PeakFilter filter;

    filter._removeUnindexed = _ui->checkUnindexed->isChecked();
    filter._removeMasked = _ui->checkMasked->isChecked();
    filter._removeUnselected = _ui->checkUnselected->isChecked();
    filter._removeIsigma = _ui->checkIsigma->isChecked();
    filter._removePValue = _ui->checkPValue->isChecked();
    filter._removeOverlapping = _ui->checkOverlapping->isChecked();
    filter._removeDmin = _ui->checkDmin->isChecked();
    filter._removeDmax = _ui->checkDmax->isChecked();
    filter._removeForbidden = _ui->checkSpaceGroup->isChecked();
    filter._removeMergedP = _ui->checkBoxMergedP->isChecked();

    filter._Isigma = _ui->spinBoxIsigma->value();
    filter._dmin = _ui->spinBoxDmin->value();
    filter._dmax = _ui->spinBoxDmax->value();
    filter._pvalue = _ui->spinBoxPValue->value();
    filter._mergedP = _ui->spinBoxMergedP->value();

    nsx::info() << "Filtering peaks...";
    _peaks = filter.apply(_peaks);
    nsx::info() << _peaks.size() << " peaks remain";

    QDialog::accept();
}

const nsx::PeakSet& DialogPeakFilter::filteredPeaks() const
{
    return _peaks;
}
