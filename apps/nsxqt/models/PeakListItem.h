#ifndef NSXQT_PEAKLISTITEM_H
#define NSXQT_PEAKLISTITEM_H

#include <memory>

#include <nsxlib/instrument/InstrumentTypes.h>

#include "InspectableTreeItem.h"
#include "models/SessionModel.h"

class PeakListItem : public InspectableTreeItem
{
public:

    explicit PeakListItem(std::shared_ptr<SessionModel> session, nsx::sptrExperiment experiment);
    QWidget* inspectItem();

private:
    std::shared_ptr<SessionModel> _session;
};

#endif // NSXQT_PEAKLISTITEM_H
