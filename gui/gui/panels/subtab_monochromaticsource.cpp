
#include "gui/panels/subtab_monochromaticsource.h"
#include <QTreeView>
#include <QHBoxLayout>

//-------------------------------------------------------------------------------------------------
//! @class SubtabMonochromatic

SubtabMonochromatic::SubtabMonochromatic()
{
    auto* layout = new QHBoxLayout;
    layout->addWidget(new QTreeView);
    setLayout(layout);
}
