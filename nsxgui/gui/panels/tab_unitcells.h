

#ifndef TAB_UNITCELLS_H
#define TAB_UNITCELLS_H

#include "nsxgui/qcr/widgets/views.h"
#include "nsxgui/gui/properties/unitcellproperty.h"

class TabUnitcells : public QcrWidget {
public:
    TabUnitcells();
private:
    UnitCellProperty* property;
};

#endif // TAB_UNITCELLS_H
