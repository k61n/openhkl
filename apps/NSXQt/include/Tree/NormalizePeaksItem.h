#ifndef NORMALIZEPEAKSITEM_H
#define NORMALIZEPEAKSITEM_H

#include "Tree/TreeItem.h"

namespace SX
{
    namespace Instrument
    {
       class Experiment;
    }
}

class NormalizePeaksItem : public TreeItem
{
public:
    NormalizePeaksItem(Experiment* experiment);
};

#endif // NORMALIZEPEAKSITEM_H
