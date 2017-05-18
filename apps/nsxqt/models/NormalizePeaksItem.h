#ifndef NORMALIZEPEAKSITEM_H
#define NORMALIZEPEAKSITEM_H

#include "models/TreeItem.h"

namespace nsx
{
    namespace Instrument
    {
       class Experiment;
    }
}

class NormalizePeaksItem : public TreeItem
{
public:
    NormalizePeaksItem(std::shared_ptr<Experiment> experiment);
};

#endif // NORMALIZEPEAKSITEM_H
