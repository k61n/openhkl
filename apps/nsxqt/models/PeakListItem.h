#pragma once

#include <vector>

#include <nsxlib/CrystalTypes.h>
#include <nsxlib/InstrumentTypes.h>
#include <nsxlib/Peak3D.h>

#include "InspectableTreeItem.h"
#include "models/SessionModel.h"

class PeakListItem : public InspectableTreeItem
{
public:
    explicit PeakListItem(const nsx::PeakList& peaks);
    QWidget* inspectItem();
    nsx::PeakList& peaks() { return _peaks; }

    void removePeak(nsx::sptrPeak3D peak);

private:
    nsx::PeakList _peaks;
};
