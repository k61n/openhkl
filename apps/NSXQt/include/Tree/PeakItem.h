#ifndef PEAKITEM_H
#define PEAKITEM_H

#include <string>

#include "TreeItem.h"
#include "Experiment.h"

class PeakItem : public TreeItem
{
public:
    explicit PeakItem(Experiment* experiment);

};

#endif // PEAKITEM_H
