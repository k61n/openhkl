
#include "nsxgui/gui/panels/tab_unitcells.h"
#include <QHBoxLayout>
#include <QTreeView>

//-------------------------------------------------------------------------------------------------
//! @class TabUnitcells

TabUnitcells::TabUnitcells()
    : QcrWidget{"unit cells"}
{
    auto* layout = new QHBoxLayout;
    layout->addWidget(new QTreeView);
    setLayout(layout);
}
