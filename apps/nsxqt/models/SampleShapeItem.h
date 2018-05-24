#ifndef NSXQT_SAMPLESHAPEITEM_H
#define NSXQT_SAMPLESHAPEITEM_H

#include <nsxlib/InstrumentTypes.h>

#include "InspectableTreeItem.h"

class SampleShapeItem : public InspectableTreeItem
{
public:
    SampleShapeItem();
    ~SampleShapeItem()=default;
    QWidget* inspectItem();
};

#endif // NSXQT_SAMPLESHAPEITEM_H
