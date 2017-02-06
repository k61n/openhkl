#ifndef DETECTORITEM_H
#define DETECTORITEM_H

#include <string>
#include <memory>

#include <QString>
#include "InspectableTreeItem.h"
#include <nsxlib/instrument/Experiment.h>

class QWidget;

class DetectorItem : public InspectableTreeItem
{
public:
    explicit DetectorItem(std::shared_ptr<Experiment> experiment);
    QWidget* inspectItem() override;

    QJsonObject toJson() override;
};

#endif // DETECTORITEM_H
