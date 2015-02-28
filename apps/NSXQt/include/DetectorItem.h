#ifndef DETECTORITEM_H
#define DETECTORITEM_H

#include <string>

#include <QString>
#include "InspectableTreeItem.h"
#include "Experiment.h"

class QWidget;

class DetectorItem : public InspectableTreeItem
{
public:
    explicit DetectorItem(Experiment* experiment);
    QWidget* inspectItem();
};

#endif // DETECTORITEM_H
