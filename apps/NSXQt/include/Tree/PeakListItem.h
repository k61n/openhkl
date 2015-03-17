#ifndef PEAKLISTITEM_H
#define PEAKLISTITEM_H

#include <string>

#include "TreeItem.h"
#include "Experiment.h"

class PeakListItem : public TreeItem
{
public:
    explicit PeakListItem(Experiment* experiment);


};

#endif // PEAKLISTITEM_H
