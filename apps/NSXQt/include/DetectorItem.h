#ifndef DETECTORITEM_H
#define DETECTORITEM_H

#include <string>

#include <QString>

#include "TreeItem.h"
#include "Experiment.h"

class DetectorItem : public TreeItem
{
public:
    explicit DetectorItem(Experiment* experiment);

};

#endif // DETECTORITEM_H
