//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/dialogs/IsotopesDatabaseDialog.h
//! @brief     Defines class IsotopesDatabaseDialog
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_GUI_DIALOGS_ISOTOPESDATABASEDIALOG_H
#define OHKL_GUI_DIALOGS_ISOTOPESDATABASEDIALOG_H

#include <QDialog>
#include <QTableView>

//! Shows a table with isotopes and their properties
class IsotopesDatabaseDialog : public QDialog {
 public:
    IsotopesDatabaseDialog();

 private:
    QTableView* isotopeDatabaseView;
    void cellClicked(const QModelIndex& index);
    void sortingChanged(int, Qt::SortOrder);
};

#endif // OHKL_GUI_DIALOGS_ISOTOPESDATABASEDIALOG_H
