//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/dialogs/PredictPeaksDialog.h
//! @brief     Defines class PredictPeaksDialog
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef GUI_DIALOGS_PREDICTPEAKSDIALOG_H
#define GUI_DIALOGS_PREDICTPEAKSDIALOG_H

#include "gui/models/PeaksTable.h"
#include <QCR/widgets/controls.h>
#include <QDialog>

//! Dialog for peak prediction
class PredictPeaksDialog : public QDialog {
 public:
    PredictPeaksDialog();

 private:
    void layout();
    void accept() override;
    void predict();

    QcrComboBox* unitCells;
    QcrComboBox* interpolation;
    QcrDoubleSpinBox* dmin;
    QcrDoubleSpinBox* dmax;
    QcrDoubleSpinBox* radius;
    QcrDoubleSpinBox* nFrames;
    QcrSpinBox* minNeighbors;
    PeaksTableView* preview;
    PeaksTableModel* model;
    nsx::PeakList peaks;
};

#endif // GUI_DIALOGS_PREDICTPEAKSDIALOG_H
