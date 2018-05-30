#pragma once

#include <memory>
#include <string>

#include <QString>

#include <nsxlib/InstrumentTypes.h>

#include "InspectableTreeItem.h"

class QWidget;

class DetectorItem : public InspectableTreeItem
{
public:

    explicit DetectorItem();
    QWidget* inspectItem() override;

    QJsonObject toJson() override;
};
