//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/dialogs/NewCellDialog.h
//! @brief     Defines class NewCellDialog
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_GUI_DIALOGS_NEWCELLDIALOG_H
#define OHKL_GUI_DIALOGS_NEWCELLDIALOG_H

#include <QComboBox>
#include <QDialog>
#include <QDoubleSpinBox>
#include <QLineEdit>

//! Dialog to assign a unit cell to a peak collection
class NewCellDialog : public QDialog {
 public:
    NewCellDialog(QStringList spaceGroups);

    QString unitCellName();
    QString spaceGroup();
    double a();
    double b();
    double c();
    double alpha();
    double beta();
    double gamma();

 private:
    QComboBox* _space_group;
    QLineEdit* _cell_name;

    QDoubleSpinBox* _a;
    QDoubleSpinBox* _b;
    QDoubleSpinBox* _c;
    QDoubleSpinBox* _alpha;
    QDoubleSpinBox* _beta;
    QDoubleSpinBox* _gamma;
};

#endif // OHKL_GUI_DIALOGS_NEWCELLDIALOG_H
