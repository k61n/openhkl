//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/dialogs/ListNameDialog.h
//! @brief     Defines class ListNameDialog
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_GUI_DIALOGS_LISTNAMEDIALOG_H
#define OHKL_GUI_DIALOGS_LISTNAMEDIALOG_H

#include <QDialog>
#include <QLineEdit>

//! Dialog to name a specific peak list
class ListNameDialog : public QDialog {
 public:
    ListNameDialog(
        QString suggestion = "", QString name = "Peak collection",
        QString header = "New peak collection");
    QString listName();

 private:
    QLineEdit* _name;
};

#endif // OHKL_GUI_DIALOGS_LISTNAMEDIALOG_H
