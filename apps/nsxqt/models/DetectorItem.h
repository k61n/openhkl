#ifndef NSXQT_DETECTORITEM_H
#define NSXQT_DETECTORITEM_H

#include <memory>
#include <string>

#include <QString>

#include <nsxlib/instrument/Experiment.h>

#include "InspectableTreeItem.h"

class QWidget;

namespace nsx {
class Experiment;
}

class DetectorItem : public InspectableTreeItem
{
public:

    using sptrExperiment = std::shared_ptr<nsx::Experiment>;

    explicit DetectorItem(sptrExperiment experiment);
    QWidget* inspectItem() override;

    QJsonObject toJson() override;
};

#endif // NSXQT_DETECTORITEM_H
