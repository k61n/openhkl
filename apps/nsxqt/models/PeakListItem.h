#ifndef NSXQT_PEAKLISTITEM_H
#define NSXQT_PEAKLISTITEM_H

#include <nsxlib/instrument/InstrumentTypes.h>

#include "InspectableTreeItem.h"

class PeakListItem : public InspectableTreeItem
{
public:

    explicit PeakListItem(nsx::sptrExperiment experiment);
    QWidget* inspectItem();

};

#endif // NSXQT_PEAKLISTITEM_H
