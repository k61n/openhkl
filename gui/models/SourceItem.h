#pragma once

#include <nsxlib/InstrumentTypes.h>

#include "InspectableTreeItem.h"

class SourceItem : public InspectableTreeItem
{
public:
    explicit SourceItem(const char* name);
    QWidget* inspectItem();
};
