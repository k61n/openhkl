//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/dialogs/ComboDialog.h
//! @brief     Defines class ComboDialog
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef NSX_GUI_DIALOGS_COMBODIALOG_H
#define NSX_GUI_DIALOGS_COMBODIALOG_H

#include <QComboBox>
#include <QDialog>

//! Dialog containing only a single QComboBox
class ComboDialog : public QDialog {
 public:
    ComboDialog(QStringList items, QString description);
    QString itemName() const;
    unsigned int itemIndex() const;

 private:
    QString _description;
    QComboBox* _items_combo;
};

#endif // NSX_GUI_DIALOGS_COMBODIALOG_H
