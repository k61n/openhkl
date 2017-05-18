#ifndef NSXQT_NORMALIZEPEAKSITEM_H
#define NSXQT_NORMALIZEPEAKSITEM_H

#include "models/TreeItem.h"

namespace nsx
{
class Experiment;
}

class NormalizePeaksItem : public TreeItem
{
public:
    NormalizePeaksItem(std::shared_ptr<nsx::Experiment> experiment);
};

#endif // NSXQT_NORMALIZEPEAKSITEM_H
