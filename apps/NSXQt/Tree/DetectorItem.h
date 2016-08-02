#ifndef DETECTORITEM_H
#define DETECTORITEM_H

#include <string>
#include <memory>

#include <QString>
#include "InspectableTreeItem.h"
#include "Experiment.h"

class QWidget;

class DetectorItem : public InspectableTreeItem
{
public:
    explicit DetectorItem(std::shared_ptr<Experiment> experiment);
    QWidget* inspectItem();
};

#endif // DETECTORITEM_H
