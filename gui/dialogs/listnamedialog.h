//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/dialogs/listnamedialog.h
//! @brief     Defines ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef NSXGUI_GUI_DIALOGS_LISTNAMEDIALOG_H
#define NSXGUI_GUI_DIALOGS_LISTNAMEDIALOG_H

#include <QCR/widgets/controls.h>
#include <QCR/widgets/modal_dialogs.h>
#include "core/crystal/CrystalTypes.h"

class ListNameDialog : public QcrModalDialog {
public:
    ListNameDialog(nsx::PeakList);
    QString listName();
    nsx::PeakList peaklist() { return list_; }

private:
    QcrLineEdit* name_;
    nsx::PeakList list_;
};

#endif // NSXGUI_GUI_DIALOGS_LISTNAMEDIALOG_H
