//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/dialogs/ListNameDialog.h
//! @brief     Defines class ListNameDialog
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef NSX_GUI_DIALOGS_LISTNAMEDIALOG_H
#define NSX_GUI_DIALOGS_LISTNAMEDIALOG_H

#include "core/peak/Peak3D.h"
#include <QDialog>
#include <QLineEdit>

//! Dialog to name a specific peak list
class ListNameDialog : public QDialog {
 public:
    ListNameDialog(QString suggestion = "");
    QString listName();

 private:
    QLineEdit* _name;
};

#endif // NSX_GUI_DIALOGS_LISTNAMEDIALOG_H
