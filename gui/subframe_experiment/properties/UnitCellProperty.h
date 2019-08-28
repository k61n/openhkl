//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/properties/UnitCellProperty.h
//! @brief     Defines class UnitCellProperty
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef GUI_SUBFRAME_EXPERIMENT_PROPERTIES_UNITCELLPROPERTY_H
#define GUI_SUBFRAME_EXPERIMENT_PROPERTIES_UNITCELLPROPERTY_H

#include <QCR/widgets/controls.h>
#include <QCR/widgets/views.h>

//! Property widget for unit cell
class UnitCellProperty : public QcrWidget {
 public:
    UnitCellProperty();

 private:
    void setUnitCellName();
    void setChemicalFormula();
    void setZValue(int z);
    void setMassDensity() const;
    void selectedCellChanged(int cell);
    void printAllInformation();

    QcrComboBox* unitcells;
    QcrLineEdit* name;
    QcrLineEdit* chemicalFormula;
    QcrLineEdit* spaceGroup;
    QcrSpinBox* z;
    QcrDoubleSpinBox* indexingTolerance;
    QcrDoubleSpinBox* a;
    QcrDoubleSpinBox* b;
    QcrDoubleSpinBox* c;
    QcrDoubleSpinBox* alpha;
    QcrDoubleSpinBox* beta;
    QcrDoubleSpinBox* gamma;
};

#endif // GUI_SUBFRAME_EXPERIMENT_PROPERTIES_UNITCELLPROPERTY_H
