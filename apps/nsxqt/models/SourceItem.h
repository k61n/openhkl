#ifndef SOURCEITEM_H
#define SOURCEITEM_H

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

#endif // SOURCEITEM_H
