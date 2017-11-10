#ifndef NSXQT_SOURCEITEM_H
#define NSXQT_SOURCEITEM_H

#include <nsxlib/InstrumentTypes.h>

#include "InspectableTreeItem.h"

class SourceItem : public InspectableTreeItem
{
public:
    explicit SourceItem(nsx::sptrExperiment experiment);
    QWidget* inspectItem();
};

#endif // NSXQT_SOURCEITEM_H
