#ifndef NSXQT_NORMALIZEPEAKSITEM_H
#define NSXQT_NORMALIZEPEAKSITEM_H

#include <nsxlib/InstrumentTypes.h>

#include "TreeItem.h"

class NormalizePeaksItem : public TreeItem
{
public:
    NormalizePeaksItem(nsx::sptrExperiment experiment);
};

#endif // NSXQT_NORMALIZEPEAKSITEM_H
