//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/dialogs/ClonePeakDialog.h
//! @brief     Defines class ClonePeakDialog
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_GUI_DIALOGS_CLONEPEAKDIALOG_H
#define OHKL_GUI_DIALOGS_CLONEPEAKDIALOG_H

#include <QComboBox>
#include <QDialog>
#include <QLineEdit>

//! Dialog to clone (deep copy) a peak collection
class ClonePeakDialog : public QDialog {
 public:
    ClonePeakDialog(QStringList collectionNames, const QString& suggestion);
    QString originalCollectionName();
    QString clonedCollectionName();

 private:
    QComboBox* _peak_collections;
    QLineEdit* _new_collection_name;
};

#endif // OHKL_GUI_DIALOGS_CLONEPEAKDIALOG_H
