//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/dialogs/DataNameDialog.h
//! @brief     Defines class DataNameDialog
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef NSX_GUI_DIALOGS_DATANAMEDIALOG_H
#define NSX_GUI_DIALOGS_DATANAMEDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QString>

//! Dialog to get the experiment and instrument names
class DataNameDialog : public QDialog {
 public:
    DataNameDialog(const QString& dataname = "", const QStringList* const datanames_cur = nullptr);
    QString dataName();
    const QStringList* const datanames; // list of current names

 private:
    QLineEdit* _dataname_ledit;
    void verify();
};

#endif // NSX_GUI_DIALOGS_DATANAMEDIALOG_H
