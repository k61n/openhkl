
#include "nsxgui/gui/panels/tab_peaklibrary.h"
#include <QHBoxLayout>
#include <QTreeView>

//-------------------------------------------------------------------------------------------------
//! @class TabPeaklibrary

TabPeaklibrary::TabPeaklibrary() : QcrWidget {"peak library"}
{
    auto* layout = new QHBoxLayout;
    layout->addWidget(new QTreeView);
    setLayout(layout);
}
