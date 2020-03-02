//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
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

#ifndef GUI_DIALOGS_LISTNAMEDIALOG_H
#define GUI_DIALOGS_LISTNAMEDIALOG_H

#include "core/peak/Peak3D.h"
#include <QDialog>
#include <QLineEdit>

//! Dialog to name a specific peak list
class ListNameDialog : public QDialog {
 public:
    ListNameDialog();
    QString listName();

 private:
    QLineEdit* name_;
};

#endif // GUI_DIALOGS_LISTNAMEDIALOG_H
