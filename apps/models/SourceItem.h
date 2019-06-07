#pragma once

#include <core/InstrumentTypes.h>

#include "InspectableTreeItem.h"

class SourceItem : public InspectableTreeItem {
public:
    explicit SourceItem(const char* name);
    QWidget* inspectItem();
};
