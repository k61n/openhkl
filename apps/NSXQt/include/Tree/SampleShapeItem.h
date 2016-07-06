#ifndef SAMPLESHAPEITEM_H
#define SAMPLESHAPEITEM_H
#include "InspectableTreeItem.h"

class SampleShapeItem : public InspectableTreeItem
{
public:
    SampleShapeItem(std::shared_ptr<Experiment> experiment);
    ~SampleShapeItem();
    QWidget* inspectItem();
};

#endif // SAMPLESHAPEITEM_H
