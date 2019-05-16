
#include "nsxgui/gui/panels/tab_data.h"
#include "nsxgui/gui/properties/numorproperty.h"
#include <QTreeView>
#include <QHBoxLayout>

//-------------------------------------------------------------------------------------------------
//! @class TabData

TabData::TabData()
    : QcrWidget{"data"}
{
    auto* layout = new QHBoxLayout;
    layout->addWidget((property = new NumorProperty));
    setLayout(layout);
}
