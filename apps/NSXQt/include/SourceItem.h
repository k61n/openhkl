#ifndef SOURCEITEM_H
#define SOURCEITEM_H

#include <string>

#include "TreeItem.h"
#include "Experiment.h"

class SourceItem : public TreeItem
{
public:
    explicit SourceItem(Experiment* experiment);

};

#endif // SOURCEITEM_H
