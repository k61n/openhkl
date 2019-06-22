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
#include <QCR/widgets/controls.h>
#include <QCR/widgets/modal_dialogs.h>

//! Dialog to name a specific peak list
class ListNameDialog : public QcrModalDialog {
 public:
    ListNameDialog(nsx::PeakList);
    QString listName();
    nsx::PeakList peaklist() { return list_; }

 private:
    QcrLineEdit* name_;
    nsx::PeakList list_;
};

#endif // GUI_DIALOGS_LISTNAMEDIALOG_H
