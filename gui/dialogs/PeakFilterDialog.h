//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/dialogs/PeakFilterDialog.h
//! @brief     Defines class PeakFilterDialog
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef GUI_DIALOGS_PEAKFILTERDIALOG_H
#define GUI_DIALOGS_PEAKFILTERDIALOG_H

#include "gui/models/PeaksTable.h"
#include <QCR/widgets/controls.h>
#include <QCR/widgets/views.h>
#include <QDialogButtonBox>

//! Dialog to filter a peak list
class PeakFilterDialog : public QcrWidget {
 public:
    PeakFilterDialog();
    ~PeakFilterDialog();

    void refreshData();

 public slots:
    void accept();
    void slotActionClicked(QAbstractButton* button);
    void slotUnitCellChanged(int index);

 private:
    void doLayout();
    void filterPeaks();

    QcrCheckBox* selected;
    QcrCheckBox* masked;
    QcrCheckBox* predicted;
    QcrCheckBox* indexedPeak;
    QcrCheckBox* sapcegroupExtincted;
    QcrCheckBox* removeOverlapping;
    QcrCheckBox* keepComplementary;
    QcrComboBox* unitCell;
    QcrDoubleSpinBox* tolerance;
    QcrDoubleSpinBox* strengthMin;
    QcrDoubleSpinBox* strengthMax;
    QcrDoubleSpinBox* drangeMin;
    QcrDoubleSpinBox* drangeMax;
    QcrDoubleSpinBox* significanceLevel;
    QcrSpinBox* minNumberPeaks;
    PeaksTableView* peaksTable;
    QGroupBox* stateBox;
    QGroupBox* byUnitCell;
    QGroupBox* strengthBox;
    QGroupBox* rangeBox;
    QGroupBox* sparseGroup;
    QGroupBox* mergeGroup;
    QDialogButtonBox* buttons;
    QcrComboBox* peakList;

    nsx::PeakList peaks_;
    PeaksTableModel* model_;
};


#endif // GUI_DIALOGS_PEAKFILTERDIALOG_H
