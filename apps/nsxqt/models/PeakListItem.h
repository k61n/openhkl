#ifndef NSXQT_PEAKLISTITEM_H
#define NSXQT_PEAKLISTITEM_H

#include <string>

#include "InspectableTreeItem.h"
#include <nsxlib/instrument/Experiment.h>

class PeakListItem : public InspectableTreeItem
{
public:
    explicit PeakListItem(std::shared_ptr<Experiment> experiment);
    QWidget* inspectItem();

};

#endif // NSXQT_PEAKLISTITEM_H
