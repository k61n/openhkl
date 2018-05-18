#ifndef NSXQT_PEAKLISTITEM_H
#define NSXQT_PEAKLISTITEM_H

#include <vector>

#include <nsxlib/CrystalTypes.h>
#include <nsxlib/InstrumentTypes.h>
#include <nsxlib/Peak3D.h>

#include "InspectableTreeItem.h"
#include "models/SessionModel.h"

class PeakListItem : public InspectableTreeItem
{
public:
    explicit PeakListItem();
    QWidget* inspectItem();
    nsx::PeakList& peaks() { return _peaks; }
private:
    nsx::PeakList _peaks;
};

#endif // NSXQT_PEAKLISTITEM_H
