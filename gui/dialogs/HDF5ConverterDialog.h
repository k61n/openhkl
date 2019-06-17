//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/dialogs/HDF5ConverterDialog.h
//! @brief     Defines class HDF5ConverterDialog
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef GUI_DIALOGS_HDF5CONVERTERDIALOG_H
#define GUI_DIALOGS_HDF5CONVERTERDIALOG_H

#include <QCR/widgets/modal_dialogs.h>
#include <QCR/widgets/controls.h>
#include <QCR/widgets/actions.h>
#include <QProgressBar>
#include <QDialogButtonBox>

//! Dialog to convert selected numors to HDF5 format
class HDF5ConverterDialog : public QcrModalDialog {
public:
    HDF5ConverterDialog();

private:
    void convert();
    void browseDirectory();

    QcrLineEdit* directory;
    QcrTextTriggerButton* browse;
    QcrLineEdit* filename;
    QProgressBar* progress;
    QDialogButtonBox* buttons;
};

#endif // GUI_DIALOGS_HDF5CONVERTERDIALOG_H
