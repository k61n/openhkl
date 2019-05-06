
#include "gui/panels/tab_data.h"
#include <QTreeView>
#include <QHBoxLayout>

//-------------------------------------------------------------------------------------------------
//! @class TabData

TabData::TabData()
{
    auto* layout = new QHBoxLayout;
    layout->addWidget(new QTreeView);
    setLayout(layout);
}
