#include <QDialogButtonBox>
#include <QDoubleSpinBox>
#include <QGroupBox>
#include <QLabel>
#include <QVBoxLayout>

#include <nsxlib/DataSet.h>
#include <nsxlib/Logger.h>
#include <nsxlib/Peak3D.h>
#include <nsxlib/PeakFilter.h>

#include "ui_PeakFilterDialog.h"
#include "DialogPeakFilter.h"

DialogPeakFilter::DialogPeakFilter(const nsx::PeakList& peaks, QWidget* parent):
    QDialog(parent),
    _ui(new Ui::PeakFilterDialog),
    _peaks(peaks),
    _badPeaks()
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
    auto&& good_peaks = filter.apply(_peaks);
    nsx::info() << good_peaks.size() << " peaks remain";

    _badPeaks.clear();

    for (auto peak: _peaks) {
        auto it = std::find(good_peaks.begin(),good_peaks.end(),peak);
        if (it == good_peaks.end()) {
            _badPeaks.add(peak);
        }
    }

    QDialog::accept();
}

const nsx::PeakList& DialogPeakFilter::badPeaks() const
{
    return _badPeaks;
}
