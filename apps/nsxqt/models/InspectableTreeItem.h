#ifndef NSXQT_INSPECTABLETREEITEM_H
#define NSXQT_INSPECTABLETREEITEM_H

#include "TreeItem.h"

namespace nsx
{
class Experiment;
}

//! Interface for tree Items for which
//! one wants to show internal properties
//! or interact.
class InspectableTreeItem : public TreeItem
{
public:
    InspectableTreeItem(std::shared_ptr<nsx::Experiment> experiment);
    ~InspectableTreeItem();
    //! Tree items can expose a QWidget that show their internal properties.
    virtual QWidget* inspectItem() = 0;
};

#endif // NSXQT_INSPECTABLETREEITEM_H
