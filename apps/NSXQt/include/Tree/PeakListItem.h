#ifndef PEAKLISTITEM_H
#define PEAKLISTITEM_H

#include <string>

#include "InspectableTreeItem.h"
#include "Experiment.h"

class PeakListItem : public InspectableTreeItem
{
public:
    explicit PeakListItem(Experiment* experiment);
    QWidget* inspectItem();

};

#endif // PEAKLISTITEM_H
