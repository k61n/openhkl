#ifndef NSXQT_SOURCEITEM_H
#define NSXQT_SOURCEITEM_H

#include <memory>

#include "InspectableTreeItem.h"

namespace nsx{
class Experiment;
}


class SourceItem : public InspectableTreeItem
{
public:
    explicit SourceItem(std::shared_ptr<nsx::Experiment> experiment);
    QWidget* inspectItem();
};

#endif // NSXQT_SOURCEITEM_H
