#pragma once

#include "InspectableTreeItem.h"

class QWidget;

class DetectorItem : public InspectableTreeItem {
public:
    explicit DetectorItem();
    QWidget* inspectItem() override;

    void openDetectorGlobalOffsetsFrame();
};
