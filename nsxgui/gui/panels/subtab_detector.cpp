
#include "nsxgui/gui/panels/subtab_detector.h"
#include "nsxgui/gui/properties/detectorproperty.h"
#include <QTreeView>
#include <QHBoxLayout>

//-------------------------------------------------------------------------------------------------
//! @class SubtabDetector

SubtabDetector::SubtabDetector()
    : QcrWidget{"detector"}
{
    auto* layout = new QHBoxLayout;
    layout->addWidget((property = new DetectorProperty));
    setLayout(layout);
}
