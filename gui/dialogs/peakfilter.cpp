//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/dialogs/peakfilter.cpp
//! @brief     Implements ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************


#include "gui/dialogs/peakfilter.h"
#include "gui/dialogs/listnamedialog.h"
#include "gui/mainwin.h"
#include "gui/models/session.h"
#include <QCR/engine/logger.h>

#include "gui/models/meta.h"

#include "core/peak/PeakFilter.h"

#include <QDialogButtonBox>
#include <QHBoxLayout>
#include <QSpacerItem>
#include <QVBoxLayout>

PeakFilter::PeakFilter() : QDialog {gGui}
{
    if (gSession->selectedExperimentNum() < 0) {
        gLogger->log("## No experiment selected");
        return;
    }

    if (gSession->selectedExperiment()->peaks()->allPeaks().empty()) {
        gLogger->log("## No peaks to filter. Find peaks first.");
        return;
    }

    peaks_ = gSession->selectedExperiment()->peaks()->allPeaks();

    setAttribute(Qt::WA_DeleteOnClose);
    doLayout();
}

PeakFilter::~PeakFilter()
{
    if (model_)
        delete model_;
}

void PeakFilter::doLayout()
{
    QVBoxLayout* whole = new QVBoxLayout(this);

    QHBoxLayout* upperLayout = new QHBoxLayout;

    QVBoxLayout* settings = new QVBoxLayout;
    stateBox = new QGroupBox("State");
    stateBox->setCheckable(true);
    stateBox->setChecked(true);
    QHBoxLayout* stateBoxLayout = new QHBoxLayout(stateBox);
    selected = new QcrCheckBox("adhoc_selected", "Selected", new QcrCell<bool>(true));
    masked = new QcrCheckBox("adhoc_masked", "Masked", new QcrCell<bool>(false));
    predicted = new QcrCheckBox("adhoc_predicted", "Predicted", new QcrCell<bool>(false));
    stateBoxLayout->addWidget(selected);
    stateBoxLayout->addWidget(masked);
    stateBoxLayout->addWidget(predicted);
    stateBoxLayout->addItem(new QSpacerItem(20, 40, QSizePolicy::Expanding, QSizePolicy::Minimum));
    settings->addWidget(stateBox);
    indexedPeak = new QcrCheckBox("adhoc_indexedPeak", "Indexed peak", new QcrCell<bool>(false));
    settings->addWidget(indexedPeak);

    byUnitCell = new QGroupBox("Indexed peaks by unit cell");
    byUnitCell->setCheckable(true);
    byUnitCell->setChecked(false);
    QHBoxLayout* byLayout = new QHBoxLayout(byUnitCell);
    unitCell = new QcrComboBox("adhoc_unitCell", new QcrCell<int>(0), QStringList {});
    byLayout->addWidget(new QLabel("Unit cell"));
    byLayout->addWidget(unitCell);
    tolerance = new QcrDoubleSpinBox("adhoc_tolerance", new QcrCell<double>(0.2), 10, 6);
    byLayout->addWidget(new QLabel("Tolerance"));
    byLayout->addWidget(tolerance);
    byLayout->addItem(new QSpacerItem(20, 40, QSizePolicy::Expanding, QSizePolicy::Minimum));
    settings->addWidget(byUnitCell);

    strengthBox = new QGroupBox("Strength (I/sigma)");
    strengthBox->setCheckable(true);
    strengthBox->setChecked(false);
    QHBoxLayout* strengthLayout = new QHBoxLayout(strengthBox);
    strengthMin = new QcrDoubleSpinBox("adhoc_strengthmin", new QcrCell<double>(1.0), 10, 6);
    strengthMax = new QcrDoubleSpinBox("adhoc_strengthmax", new QcrCell<double>(3.0), 10, 6);
    strengthLayout->addWidget(new QLabel("Min"));
    strengthLayout->addWidget(strengthMin);
    strengthLayout->addWidget(new QLabel("Max"));
    strengthLayout->addWidget(strengthMax);
    strengthLayout->addItem(new QSpacerItem(20, 30, QSizePolicy::Expanding, QSizePolicy::Minimum));
    settings->addWidget(strengthBox);

    rangeBox = new QGroupBox("d range");
    rangeBox->setCheckable(true);
    rangeBox->setChecked(false);
    QHBoxLayout* rangeLayout = new QHBoxLayout(rangeBox);
    drangeMin = new QcrDoubleSpinBox("adhoc_drangemin", new QcrCell<double>(0.0), 10, 6);
    drangeMax = new QcrDoubleSpinBox("adhoc_drangemax", new QcrCell<double>(100.0), 10, 6);
    rangeLayout->addWidget(new QLabel("Min"));
    rangeLayout->addWidget(drangeMin);
    rangeLayout->addWidget(new QLabel("Max"));
    rangeLayout->addWidget(drangeMax);
    rangeLayout->addItem(new QSpacerItem(20, 30, QSizePolicy::Expanding, QSizePolicy::Minimum));
    settings->addWidget(rangeBox);

    sapcegroupExtincted =
        new QcrCheckBox("adhoc_extincted", "Space-group extincted", new QcrCell<bool>(false));
    settings->addWidget(sapcegroupExtincted);
    sparseGroup = new QGroupBox("Sparse dataset");
    sparseGroup->setCheckable(true);
    sparseGroup->setChecked(false);
    QHBoxLayout* sparse = new QHBoxLayout(sparseGroup);
    sparse->addWidget(new QLabel("Min number of peaks"));
    minNumberPeaks = new QcrSpinBox("adhoc_numPeaks", new QcrCell<int>(0), 5);
    sparse->addWidget(minNumberPeaks);
    sparse->addItem(new QSpacerItem(20, 40, QSizePolicy::Expanding, QSizePolicy::Minimum));
    settings->addWidget(sparseGroup);

    mergeGroup = new QGroupBox("Merged peak significance");
    mergeGroup->setCheckable(true);
    mergeGroup->setChecked(false);
    QHBoxLayout* mergebox = new QHBoxLayout(mergeGroup);
    mergebox->addWidget(new QLabel("Significant level"));
    significanceLevel = new QcrDoubleSpinBox("adhoc_level", new QcrCell<double>(0.99), 10, 6);
    mergebox->addWidget(significanceLevel);
    mergebox->addItem(new QSpacerItem(20, 40, QSizePolicy::Expanding, QSizePolicy::Minimum));
    settings->addWidget(mergeGroup);

    removeOverlapping = new QcrCheckBox(
        "adhoc_removeOverlapping", "Remove overlapping peaks", new QcrCell<bool>(false));
    settings->addWidget(removeOverlapping);
    keepComplementary = new QcrCheckBox(
        "adhoc_keepComplementary", "Keep the complementary selection", new QcrCell<bool>(false));
    settings->addWidget(keepComplementary);
    upperLayout->addLayout(settings);

    QVBoxLayout* tablelayout = new QVBoxLayout;
    peakList = new QcrComboBox(
        "adhoc_peakListsPeakFilter", new QcrCell<int>(0),
        gSession->selectedExperiment()->peaks()->peaklistNames());
    tablelayout->addWidget(peakList);
    model_ = new PeaksTableModel(
        "adhoc_filterModel", gSession->selectedExperiment()->experiment(), peaks_);
    peaksTable = new PeaksTableView;
    peaksTable->setModel(model_);
    tablelayout->addWidget(peaksTable);
    upperLayout->addLayout(tablelayout);

    whole->addLayout(upperLayout);
    buttons = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel | QDialogButtonBox::Apply, Qt::Horizontal);
    whole->addWidget(buttons);

    peakList->setHook([this](int i) {
        peaks_ = gSession->selectedExperiment()->peaks()->selectedPeakLists(i)->getAllListPeaks();
        model_->setPeaks(peaks_);
    });
    connect(buttons, &QDialogButtonBox::clicked, this, &PeakFilter::slotActionClicked);

    show();
}

void PeakFilter::filterPeaks()
{
    // TODO: filter peaks

    nsx::PeakList filtered_peaks = peaks_;

    nsx::PeakFilter peak_filter;

    if (stateBox->isChecked()) {
        filtered_peaks = peak_filter.selected(filtered_peaks, selected->isChecked());
        filtered_peaks = peak_filter.masked(filtered_peaks, masked->isChecked());
        filtered_peaks = peak_filter.predicted(filtered_peaks, predicted->isChecked());
    }

    if (indexedPeak->isChecked())
        filtered_peaks = peak_filter.indexed(filtered_peaks);

    if (byUnitCell->isChecked()) {
        if (unitCell->count() > 0) {
            auto unit_cell = unitCell->itemData(unitCell->currentIndex(), Qt::UserRole)
                                 .value<nsx::sptrUnitCell>();
            filtered_peaks = peak_filter.indexed(filtered_peaks, *unit_cell, tolerance->value());
        }
    }

    if (strengthBox->isChecked()) {
        double smin = strengthMin->value();
        double smax = strengthMax->value();

        filtered_peaks = peak_filter.strength(filtered_peaks, smin, smax);
    }

    if (rangeBox->isChecked()) {
        double d_min = drangeMin->value();
        double d_max = drangeMax->value();

        filtered_peaks = peak_filter.dRange(filtered_peaks, d_min, d_max);
    }

    if (sapcegroupExtincted->isChecked())
        filtered_peaks = peak_filter.extincted(filtered_peaks);

    if (sparseGroup->isChecked()) {
        size_t min_num_peaks = static_cast<size_t>(minNumberPeaks->value());

        filtered_peaks = peak_filter.sparseDataSet(filtered_peaks, min_num_peaks);
    }

    if (mergeGroup->isChecked()) {
        double significance_level = significanceLevel->value();

        filtered_peaks = peak_filter.mergedPeaksSignificance(filtered_peaks, significance_level);
    }

    if (removeOverlapping->isChecked())
        filtered_peaks = peak_filter.overlapping(filtered_peaks);

    if (keepComplementary->isChecked())
        filtered_peaks = peak_filter.complementary(peaks_, filtered_peaks);

    model_->setPeaks(filtered_peaks);
}

void PeakFilter::accept()
{
    auto& filtered_peaks = model_->peaks();

    if (!filtered_peaks.empty()) {
        std::unique_ptr<ListNameDialog> dlg(new ListNameDialog(filtered_peaks));

        if (!dlg->exec())
            return;

        gSession->selectedExperiment()
            ->peaks()
            ->selectedPeakLists(peakList->getValue())
            ->addFilteredPeaks(dlg->listName(), filtered_peaks);

        QString message = "Applied peak filters on selected peaks. Remains ";
        message += QString::number(filtered_peaks.size());
        message += " out of ";
        message += QString::number(peaks_.size());
        message += " peaks";
        gLogger->log(message);
    }

    QDialog::accept();
}

void PeakFilter::slotActionClicked(QAbstractButton* button)
{
    auto button_role = buttons->standardButton(button);

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
