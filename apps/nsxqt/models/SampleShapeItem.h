#ifndef NSXQT_SAMPLESHAPEITEM_H
#define NSXQT_SAMPLESHAPEITEM_H

#include "InspectableTreeItem.h"

namespace nsx {
class Experiment;
}

class SampleShapeItem : public InspectableTreeItem
{
public:
    SampleShapeItem(std::shared_ptr<nsx::Experiment> experiment);
    ~SampleShapeItem();
    QWidget* inspectItem();
};

#endif // NSXQT_SAMPLESHAPEITEM_H
