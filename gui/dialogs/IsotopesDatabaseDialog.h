//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
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

#ifndef GUI_DIALOGS_ISOTOPESDATABASEDIALOG_H
#define GUI_DIALOGS_ISOTOPESDATABASEDIALOG_H

#include <QDialog>
#include <QTableView>

class IsotopesDatabaseDialog : public QDialog {
public:
    IsotopesDatabaseDialog();

private:
    QTableView* isotopeDatabaseView;
    void cellClicked(const QModelIndex& index);
    void sortingChanged(int, Qt::SortOrder);
};

#endif //GUI_DIALOGS_ISOTOPESDATABASEDIALOG_H
