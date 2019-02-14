#pragma once

#include <core/CrystalTypes.h>
#include <core/InstrumentTypes.h>
#include <core/Peak3D.h>

#include "InspectableTreeItem.h"
#include "SessionModel.h"

class PeakListItem : public InspectableTreeItem
{
public:

    explicit PeakListItem(const nsx::PeakList& peaks);

    QWidget* inspectItem();

    nsx::PeakList& peaks();

    void removePeak(nsx::sptrPeak3D peak);

private:

    nsx::PeakList _peaks;
};
