//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/dialogs/ConfirmOverwriteDialog.h
//! @brief     Defines class ConfirmOverwriteDialog
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef NSX_GUI_DIALOGS_CONFIRMOVERWRITEDIALOG_H
#define NSX_GUI_DIALOGS_CONFIRMOVERWRITEDIALOG_H

#include <QDialog>
#include <QString>

//! Dialog to get the experiment and instrument names
class ConfirmOverwriteDialog : public QDialog {
 public:
    ConfirmOverwriteDialog(const QString& name);
};

#endif // NSX_GUI_DIALOGS_CONFIRMOVERWRITEDIALOG_H
