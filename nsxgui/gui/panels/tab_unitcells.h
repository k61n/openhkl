

#ifndef NSXGUI_GUI_PANELS_TAB_UNITCELLS_H
#define NSXGUI_GUI_PANELS_TAB_UNITCELLS_H

#include "nsxgui/gui/properties/unitcellproperty.h"
#include <QCR/widgets/views.h>

class TabUnitcells : public QcrWidget {
public:
    TabUnitcells();

private:
    UnitCellProperty* property;
};

#endif // NSXGUI_GUI_PANELS_TAB_UNITCELLS_H
