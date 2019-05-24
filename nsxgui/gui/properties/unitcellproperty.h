
#ifndef NSXGUI_GUI_PROPERTIES_UNITCELLPROPERTY_H
#define NSXGUI_GUI_PROPERTIES_UNITCELLPROPERTY_H

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

#endif // NSXGUI_GUI_PROPERTIES_UNITCELLPROPERTY_H
