
#include "nsxgui/gui/panels/tab_unitcells.h"
#include "nsxgui/gui/properties/unitcellproperty.h"
#include <QHBoxLayout>
#include <QTreeView>

//-------------------------------------------------------------------------------------------------
//! @class TabUnitcells

TabUnitcells::TabUnitcells()
    : QcrWidget{"unit cells"}
{
    auto* layout = new QHBoxLayout;
    layout->addWidget((property = new UnitCellProperty));
    setLayout(layout);
}
