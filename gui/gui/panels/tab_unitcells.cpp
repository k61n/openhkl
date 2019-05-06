
#include "gui/panels/tab_unitcells.h"
#include <QHBoxLayout>
#include <QTreeView>

//-------------------------------------------------------------------------------------------------
//! @class TabUnitcells

TabUnitcells::TabUnitcells()
{
    auto* layout = new QHBoxLayout;
    layout->addWidget(new QTreeView);
    setLayout(layout);
}
