#ifndef NSXQT_NORMALIZEPEAKSITEM_H
#define NSXQT_NORMALIZEPEAKSITEM_H

#include <nsxlib/instrument/InstrumentTypes.h>

#include "models/TreeItem.h"

class NormalizePeaksItem : public TreeItem
{
public:
    NormalizePeaksItem(nsx::sptrExperiment experiment);
};

#endif // NSXQT_NORMALIZEPEAKSITEM_H
