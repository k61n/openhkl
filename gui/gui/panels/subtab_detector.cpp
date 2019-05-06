
#include "gui/panels/subtab_detector.h"
#include <QTreeView>
#include <QHBoxLayout>

//-------------------------------------------------------------------------------------------------
//! @class SubtabDetector

SubtabDetector::SubtabDetector()
{
    auto* layout = new QHBoxLayout;
    layout->addWidget(new QTreeView);
    setLayout(layout);
}
