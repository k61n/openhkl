#pragma once

#include <nsxlib/InstrumentTypes.h>

#include "InspectableTreeItem.h"

class SampleShapeItem : public InspectableTreeItem
{
public:
    SampleShapeItem();
    ~SampleShapeItem()=default;
    QWidget* inspectItem();
};
