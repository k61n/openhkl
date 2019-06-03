//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/dialogs/peakfilter.h
//! @brief     Defines ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************


#ifndef NSXGUI_GUI_DIALOGS_PEAKFILTER_H
#define NSXGUI_GUI_DIALOGS_PEAKFILTER_H

#include "gui/models/experimentmodel.h"
#include "gui/models/peakstable.h"
#include <QCR/widgets/controls.h>
#include <QCR/widgets/views.h>

#include <QDialogButtonBox>
#include <QGroupBox>

class PeakFilter : public QDialog {
public:
    PeakFilter();
    ~PeakFilter();

public slots:
    void accept() override;
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


#endif // NSXGUI_GUI_DIALOGS_PEAKFILTER_H
