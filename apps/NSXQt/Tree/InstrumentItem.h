#ifndef INSTRUMENTITEM_H
#define INSTRUMENTITEM_H

#include <string>

#include "Experiment.h"
#include <Tree/TreeItem.h>

class InstrumentItem : public TreeItem
{
public:
    explicit InstrumentItem(std::shared_ptr<Experiment> experiment);

};

#endif // INSTRUMENTITEM_H
