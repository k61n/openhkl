
#include "nsxgui/gui/panels/subtab_sample.h"
#include "nsxgui/gui/properties/sampleshapeproperties.h"
#include <QHBoxLayout>
#include <QTreeView>

//-------------------------------------------------------------------------------------------------
//! @class SubtabSample

SubtabSample::SubtabSample() : QcrWidget {"sample"}
{
    auto* layout = new QHBoxLayout;
    layout->addWidget((property = new SampleShapeProperties));
    setLayout(layout);
}
