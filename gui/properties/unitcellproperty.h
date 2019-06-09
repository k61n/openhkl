//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/properties/unitcellproperty.h
//! @brief     Defines ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************


#ifndef GUI_PROPERTIES_UNITCELLPROPERTY_H
#define GUI_PROPERTIES_UNITCELLPROPERTY_H

#include <QCR/widgets/controls.h>
#include <QCR/widgets/views.h>

class UnitCellProperty : public QcrWidget {
public:
    UnitCellProperty();

private:
    void setUnitCellName();
    void getLatticeParams();
    void setLatticeParams();
    void setChemicalFormula();
    void setSpaceGroup(QString sg);
    void setZValue(int z);
    void setMassDensity() const;
    void activateSpaceGroupCompletion(QString sg);
    void setIndexingTolerance(double);
    void update();
    void updateCellParameters();

    QcrLineEdit* name;
    QcrLineEdit* chemicalFormula;
    QcrComboBox* spaceGroup;
    QcrSpinBox* z;
    QcrDoubleSpinBox* indexingTolerance;
    QcrDoubleSpinBox* a;
    QcrDoubleSpinBox* b;
    QcrDoubleSpinBox* c;
    QcrDoubleSpinBox* alpha;
    QcrDoubleSpinBox* beta;
    QcrDoubleSpinBox* gamma;
};

#endif // GUI_PROPERTIES_UNITCELLPROPERTY_H
