//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/subframe_experiment/properties/UnitCellProperty.h
//! @brief     Defines class UnitCellProperty
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef NSX_GUI_SUBFRAME_EXPERIMENT_PROPERTIES_UNITCELLPROPERTY_H
#define NSX_GUI_SUBFRAME_EXPERIMENT_PROPERTIES_UNITCELLPROPERTY_H

#include <QComboBox>
#include <QDoubleSpinBox>
#include <QLineEdit>
#include <QPushButton>
#include <QSpinBox>
#include <QWidget>

class CellComboBox;

//! Property widget for unit cell
class UnitCellProperty : public QWidget {
 public:
    //! Constructor
    UnitCellProperty();
    //! Public call to refresh the widget
    void refreshInput();

 private:
    //! Set up the GUI size policies
    void setSizePolicies();
    //! Change the name of the unit cell
    void setUnitCellName();
    //! Change the chemical formula
    void setChemicalFormula();
    //! Set the z value
    void setZValue(int z);
    //! Set the mass density
    void setMassDensity() const;
    //! Selection of unit cell changed
    void selectedCellChanged(int cell);
    //! Display all informations in QDebug
    void printAllInformation();
    //! Add unit cell (User defined)
    void addUnitCell();
    //! Remove a unit cell (deletion)
    void removeUnitCell();
    //! Reset the values
    void resetFields();
    //! Reset the values
    void setInputEnabled(bool state);

 private:
    CellComboBox* unitcells;
    QLineEdit* name;
    QLineEdit* chemicalFormula;
    QLineEdit* spaceGroup;
    QSpinBox* z;
    QDoubleSpinBox* indexingTolerance;
    QDoubleSpinBox* a;
    QDoubleSpinBox* b;
    QDoubleSpinBox* c;
    QDoubleSpinBox* alpha;
    QDoubleSpinBox* beta;
    QDoubleSpinBox* gamma;

    QPushButton* _add;
    QPushButton* _remove;

    QSizePolicy* _size_policy_widgets;
    QSizePolicy* _size_policy_box;
    QSizePolicy* _size_policy_right;
    QSizePolicy* _size_policy_fixed;
};

#endif // NSX_GUI_SUBFRAME_EXPERIMENT_PROPERTIES_UNITCELLPROPERTY_H
