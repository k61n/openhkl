//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/dialogs/UnitCellDialog.h
//! @brief     Defines class UnitCellDialog
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_GUI_DIALOGS_UNITCELLDIALOG_H
#define OHKL_GUI_DIALOGS_UNITCELLDIALOG_H

#include <QComboBox>
#include <QDialog>
#include <QLineEdit>

//! Dialog to assign a unit cell to a peak collection
class UnitCellDialog : public QDialog {
 public:
    UnitCellDialog(QString name, QStringList collectionNames, QStringList spaceGroups);
    QString unitCellName();
    QString peakCollectionName();
    QString spaceGroup();

 private:
    QComboBox* _peak_collections;
    QComboBox* _space_group;
    QLineEdit* _cell_name;
};

#endif // OHKL_GUI_DIALOGS_UNITCELLDIALOG_H
