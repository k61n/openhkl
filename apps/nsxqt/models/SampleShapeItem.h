#ifndef SAMPLESHAPEITEM_H
#define SAMPLESHAPEITEM_H

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

#endif // SAMPLESHAPEITEM_H
