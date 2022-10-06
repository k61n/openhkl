//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/dialogs/ExperimentDialog.h
//! @brief     Defines class ExperimentDialog
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_GUI_DIALOGS_EXPERIMENTDIALOG_H
#define OHKL_GUI_DIALOGS_EXPERIMENTDIALOG_H

#include <QComboBox>
#include <QDialog>
#include <QLineEdit>

//! Dialog to get the experiment and instrument names
class ExperimentDialog : public QDialog {
 public:
    ExperimentDialog(QString exp_name = "");
    QString experimentName();
    QString instrumentName();

 private:
    QComboBox* instruments;
    QLineEdit* experiment;
};

#endif // OHKL_GUI_DIALOGS_EXPERIMENTDIALOG_H
