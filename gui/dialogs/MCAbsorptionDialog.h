//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/dialogs/MCAbsorptionDialog.h
//! @brief     Declares class MCAbsorptionDialog
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef GUI_DIALOGS_MCABSORPTIONDIALOG_H
#define GUI_DIALOGS_MCABSORPTIONDIALOG_H

#include <QCR/engine/cell.h>
#include <QDialog>
#include <QProgressBar>

class MCAbsorptionDialog : public QDialog {

public:
    MCAbsorptionDialog();

private:
    void runMCAbsorption();

    QProgressBar* progressBar;
    QcrCell<int>* selUnitCell;
    QcrCell<int>* numRaysCell;
};

#endif // GUI_DIALOGS_MCABSORPTIONDIALOG_H
