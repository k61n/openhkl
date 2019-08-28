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

#include "core/analyse/PeakFilter.h"
#include "gui/MainWin.h"
#include "gui/dialogs/ListNameDialog.h"
#include "gui/models/Meta.h"
#include "gui/models/Session.h"

PeakFilterDialog::PeakFilterDialog() : QWidget()
{

    setSizePolicies();
    _main_layout = new QHBoxLayout(this);
    _right_element = new QSplitter(Qt::Vertical , this);


    // if (gSession->selectedExperimentNum() < 0) {
    //     gLogger->log("## No experiment selected");
    //     return;
    // }

    // if (gSession->selectedExperiment()->getPeakListNames().empty()) {
    //     gLogger->log("## No peaks to filter. Find peaks first.");
    //     return;
    // }

    // peaks_ = gSession->selectedExperiment()->getPeaks(0, 0)->peaks_;

    // setAttribute(Qt::WA_DeleteOnClose);
    // doLayout();
}

PeakFilterDialog::~PeakFilterDialog()
{}

void PeakFilterDialog::setSizePolicies()
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

void PeakFilterDialog::setInputUp()
{


}

void PeakFilterDialog::setStateUp()
{
    _state_box = new QGroupBox("State");
    _state_box->setCheckable(true);
    _state_box->setChecked(true);
    _state_box->setSizePolicy(*_size_policy_box);

    QGridLayout* _state_box_layout = new QGridLayout(_state_box);

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

    _state_box_layout->addWidget(_selected, 0,0,1,2);
    _state_box_layout->addWidget(_masked, 1,0,1,2);
    _state_box_layout->addWidget(_predicted, 2,0,1,2);
    _state_box_layout->addWidget(_indexed_peaks, 3,0,1,2);

    _left_layout->addWidget(_state_box);
}

void PeakFilterDialog::setUnitCellUp()
{


}

void PeakFilterDialog::setStrengthUp()
{


}

void PeakFilterDialog::setRangeUp()
{


}

void PeakFilterDialog::setMergeUp()
{


}



    // QVBoxLayout* whole = new QVBoxLayout(this);
    // QHBoxLayout* upperLayout = new QHBoxLayout;



    // _unit_cell_box = new QGroupBox("Indexed peaks by unit cell");
    // _unit_cell_box->setCheckable(true);
    // _unit_cell_box->setChecked(false);
    // QHBoxLayout* byLayout = new QHBoxLayout(_unit_cell_box);
    // // _unit_cell = new QComboBox();
    // // byLayout->addWidget(new QLabel("Unit cell"));
    // // byLayout->addWidget(_unit_cell);
    // _tolerance = new QcrDoubleSpinBox("adhoc_tolerance", new QcrCell<double>(0.2), 10, 6);
    // byLayout->addWidget(new QLabel("Tolerance"));
    // byLayout->addWidget(_tolerance);
    // byLayout->addItem(new QSpacerItem(20, 40, QSizePolicy::Expanding, QSizePolicy::Minimum));
    // settings->addWidget(_unit_cell_box);

    // _strength_box = new QGroupBox("Strength (I/sigma)");
    // _strength_box->setCheckable(true);
    // _strength_box->setChecked(false);
    // QHBoxLayout* strengthLayout = new QHBoxLayout(_strength_box);
    // _strength_min = new QcrDoubleSpinBox("adhoc_strengthmin", new QcrCell<double>(1.0), 10, 6);
    // _strength_max = new QcrDoubleSpinBox("adhoc_strengthmax", new QcrCell<double>(3.0), 10, 6);
    // strengthLayout->addWidget(new QLabel("Min"));
    // strengthLayout->addWidget(_strength_min);
    // strengthLayout->addWidget(new QLabel("Max"));
    // strengthLayout->addWidget(_strength_max);
    // strengthLayout->addItem(new QSpacerItem(20, 30, QSizePolicy::Expanding, QSizePolicy::Minimum));
    // settings->addWidget(_strength_box);

    // _d_range_box = new QGroupBox("d range");
    // _d_range_box->setCheckable(true);
    // _d_range_box->setChecked(false);
    // QHBoxLayout* rangeLayout = new QHBoxLayout(_d_range_box);
    // _d_range_min = new QcrDoubleSpinBox("adhoc_drangemin", new QcrCell<double>(0.0), 10, 6);
    // _d_range_max = new QcrDoubleSpinBox("adhoc_drangemax", new QcrCell<double>(100.0), 10, 6);
    // rangeLayout->addWidget(new QLabel("Min"));
    // rangeLayout->addWidget(_d_range_min);
    // rangeLayout->addWidget(new QLabel("Max"));
    // rangeLayout->addWidget(_d_range_max);
    // rangeLayout->addItem(new QSpacerItem(20, 30, QSizePolicy::Expanding, QSizePolicy::Minimum));
    // settings->addWidget(_d_range_box);

    // _extinct_spacegroup =
    //     new QCheckBox("adhoc_extincted", "Space-group extincted", new QcrCell<bool>(false));
    // settings->addWidget(_extinct_spacegroup);

    // _sparse_box = new QGroupBox("Sparse dataset");
    // _sparse_box->setCheckable(true);
    // _sparse_box->setChecked(false);
    // QHBoxLayout* sparse = new QHBoxLayout(_sparse_box);
    // sparse->addWidget(new QLabel("Min number of peaks"));
    // _min_number_peaks = new QcrSpinBox("adhoc_numPeaks", new QcrCell<int>(0), 5);
    // sparse->addWidget(_min_number_peaks);
    // sparse->addItem(new QSpacerItem(20, 40, QSizePolicy::Expanding, QSizePolicy::Minimum));
    // settings->addWidget(_sparse_box);

    // _merge_box = new QGroupBox("Merged peak significance");
    // _merge_box->setCheckable(true);
    // _merge_box->setChecked(false);
    // QHBoxLayout* mergebox = new QHBoxLayout(_merge_box);
    // mergebox->addWidget(new QLabel("Significant level"));
    // _significance_level = new QcrDoubleSpinBox("adhoc_level", new QcrCell<double>(0.99), 10, 6);
    // mergebox->addWidget(_significance_level);
    // mergebox->addItem(new QSpacerItem(20, 40, QSizePolicy::Expanding, QSizePolicy::Minimum));
    // settings->addWidget(_merge_box);

    // _remove_overlaping = new QCheckBox(
    //     "adhoc_removeOverlapping", "Remove overlapping peaks", new QcrCell<bool>(false));
    // settings->addWidget(_remove_overlaping);
    // _keep_complementary = new QCheckBox(
    //     "adhoc_keepComplementary", "Keep the complementary selection", new QcrCell<bool>(false));
    // settings->addWidget(_keep_complementary);
    // upperLayout->addLayout(settings, 1);

    // QVBoxLayout* tablelayout = new QVBoxLayout;
    // // peakList = new QComboBox(
    // //     "adhoc_peakListsPeakFilter", new QcrCell<int>(0),
    // //     gSession->selectedExperiment()->getPeakListNames());
    // // tablelayout->addWidget(peakList);
    // // model_ = new PeaksTableModel(
    // //     "adhoc_filterModel", gSession->selectedExperiment()->experiment(), peaks_);
    // _peaks_table = new PeaksTableView;
    // // peaksTable->setModel(model_);
    // tablelayout->addWidget(_peaks_table);
    // upperLayout->addLayout(tablelayout, 2);

    // // whole->addLayout(upperLayout);
    // // buttons = new QDialogButtonBox(
    // //     QDialogButtonBox::Ok | QDialogButtonBox::Apply, Qt::Horizontal);
    // // whole->addWidget(buttons);

    // // peakList->setHook([this](int) {
    // //     peaks_ = gSession->selectedExperiment()->getPeaks(peakList->currentText())->peaks_;
    // //     model_->setPeaks(peaks_);
    // // });
    // // connect(buttons, &QDialogButtonBox::clicked, this, &PeakFilterDialog::slotActionClicked);
// }

void PeakFilterDialog::refreshData()
{
    if ((gSession->selectedExperimentNum() < 0) ||
            (gSession->selectedExperiment()->getPeakListNames().empty()))
        return;

    // if (!model_) {
    //     return;
        // model_ = new PeaksTableModel("peakFilterPeakModel",
        //                              gSession->selectedExperiment()->experiment(),
        //                              gSession->selectedExperiment()->getPeaks()->peaks_);
        // peaksTable->setModel(model_);
    // }
}

void PeakFilterDialog::filterPeaks()
{
    // nsx::PeakList filtered_peaks = peaks_;

    // nsx::PeakFilter peak_filter;

    // if (_state_box->isChecked()) {
    //     filtered_peaks = peak_filter.selected(filtered_peaks, selected->isChecked());
    //     filtered_peaks = peak_filter.masked(filtered_peaks, masked->isChecked());
    //     filtered_peaks = peak_filter.predicted(filtered_peaks, predicted->isChecked());
    // }

    // if (indexedPeak->isChecked())
    //     filtered_peaks = peak_filter.indexed(filtered_peaks);

    // if (_unit_cell_box->isChecked()) {
    //     if (_unit_cell->count() > 0) {
    //         nsx::sptrUnitCell unit_cell = _unit_cell->itemData(_unit_cell->currentIndex(), Qt::UserRole)
    //                                           .value<nsx::sptrUnitCell>();
    //         filtered_peaks = peak_filter.indexed(filtered_peaks, *unit_cell, tolerance->value());
    //     }
    // }

    // if (_strength_box->isChecked()) {
    //     double smin = _strength_min->value();
    //     double smax = _strength_max->value();

    //     filtered_peaks = peak_filter.strength(filtered_peaks, smin, smax);
    // }

    // if (_d_range_box->isChecked()) {
    //     double d_min = _d_range_min->value();
    //     double d_max = _d_range_max->value();

    //     filtered_peaks = peak_filter.dRange(filtered_peaks, d_min, d_max);
    // }

    // if (_extinct_spacegroup->isChecked())
    //     filtered_peaks = peak_filter.extincted(filtered_peaks);

    // if (_sparse_box->isChecked()) {
    //     size_t min_num_peaks = static_cast<size_t>(_min_number_peaks->value());

    //     filtered_peaks = peak_filter.sparseDataSet(filtered_peaks, min_num_peaks);
    // }

    // if (_merge_box->isChecked()) {
    //     double significance_level = significanceLevel->value();

    //     filtered_peaks = peak_filter.mergedPeaksSignificance(filtered_peaks, significance_level);
    // }

    // if (_remove_overlaping->isChecked())
    //     filtered_peaks = peak_filter.overlapping(filtered_peaks);

    // if (_keep_complementary->isChecked())
    //     filtered_peaks = peak_filter.complementary(peaks_, filtered_peaks);

    // model_->setPeaks(filtered_peaks);
}

void PeakFilterDialog::accept()
{
    // const nsx::PeakList& filtered_peaks = model_->peaks();

    // if (!filtered_peaks.empty()) {
    //     std::unique_ptr<ListNameDialog> dlg(new ListNameDialog(filtered_peaks));

    //     if (!dlg->exec())
    //         return;

    //     Peaks* peaks = new Peaks(filtered_peaks, dlg->listName(), listtype::FILTERED, "unknown");
    //     const Peaks* parent = gSession->selectedExperiment()->getPeaks(peakList->currentText());
    //     peaks->parent = peakList->currentText();
    //     peaks->convolutionkernel_ = parent->convolutionkernel_;
    //     peaks->file_ = parent->file_;
    //     gSession->selectedExperiment()->addPeaks(peaks, peakList->currentText());

    //     QString message = "Applied peak filters on selected peaks. Remains ";
    //     message += QString::number(filtered_peaks.size());
    //     message += " out of ";
    //     message += QString::number(peaks_.size());
    //     message += " peaks";
    //     gLogger->log(message);
    // }

    // QDialog::accept();
}

void PeakFilterDialog::slotActionClicked(QAbstractButton* button)
{
//     auto button_role = buttons->standardButton(button);

//     switch (button_role) {
//         case QDialogButtonBox::StandardButton::Apply: {
//             filterPeaks();
//             break;
//         }
//         case QDialogButtonBox::StandardButton::Ok: {
//             accept();
//             break;
//         }
//         default: {
//             return;
//         }
//     }
}
