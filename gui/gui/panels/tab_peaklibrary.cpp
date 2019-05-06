
#include "gui/panels/tab_peaklibrary.h"
#include <QTreeView>
#include <QHBoxLayout>

//-------------------------------------------------------------------------------------------------
//! @class TabPeaklibrary

TabPeaklibrary::TabPeaklibrary()
{
    auto* layout = new QHBoxLayout;
    layout->addWidget(new QTreeView);
    setLayout(layout);
}

