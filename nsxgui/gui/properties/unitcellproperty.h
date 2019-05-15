
#ifndef UNITCELLPROPERTY_H
#define UNITCELLPROPERTY_H

#include "nsxgui/qcr/widgets/views.h"
#include "nsxgui/qcr/widgets/controls.h"

class UnitCellProperty : public QcrWidget {
public:
    UnitCellProperty();
private:
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

#endif //UNITCELLPROPERTY_H
