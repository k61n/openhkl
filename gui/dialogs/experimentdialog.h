//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/dialogs/experimentdialog.h
//! @brief     Defines ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************


#ifndef GUI_DIALOGS_EXPERIMENTDIALOG_H
#define GUI_DIALOGS_EXPERIMENTDIALOG_H

#include <QCR/widgets/controls.h>
#include <QCR/widgets/modal_dialogs.h>

class ExperimentDialog : public QcrModalDialog {
public:
    ExperimentDialog();
    QString experimentName();
    QString instrumentName();

private:
    QcrComboBox* instruments;
    QcrLineEdit* experiment;
};

#endif // GUI_DIALOGS_EXPERIMENTDIALOG_H
