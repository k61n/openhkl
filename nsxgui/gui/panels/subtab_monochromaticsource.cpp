
#include "nsxgui/gui/panels/subtab_monochromaticsource.h"
#include "nsxgui/gui/properties/sourceproperty.h"
#include <QHBoxLayout>
#include <QTreeView>

//-------------------------------------------------------------------------------------------------
//! @class SubtabMonochromatic

SubtabMonochromatic::SubtabMonochromatic() : QcrWidget {"monochromatic source"}
{
    auto* layout = new QHBoxLayout;
    layout->addWidget((property = new SourceProperty));
    setLayout(layout);
}
