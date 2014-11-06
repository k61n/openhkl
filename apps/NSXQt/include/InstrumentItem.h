#ifndef INSTRUMENTITEM_H
#define INSTRUMENTITEM_H

#include <string>

#include <TreeItem.h>
#include "Experiment.h"

class InstrumentItem : public TreeItem
{
public:
    explicit InstrumentItem(Experiment* experiment);

signals:

};

#endif // INSTRUMENTITEM_H
