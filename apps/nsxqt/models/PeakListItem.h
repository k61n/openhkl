#ifndef NSXQT_PEAKLISTITEM_H
#define NSXQT_PEAKLISTITEM_H

#include <string>

#include "InspectableTreeItem.h"
#include <nsxlib/instrument/Experiment.h>

namespace nsx
{
class Experiment;
}

class PeakListItem : public InspectableTreeItem
{
public:

    using sptrExperiment = std::shared_ptr<nsx::Experiment>;

    explicit PeakListItem(sptrExperiment experiment);
    QWidget* inspectItem();

};

#endif // NSXQT_PEAKLISTITEM_H
