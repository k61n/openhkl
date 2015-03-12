#ifndef INSPECTABLETREEITEM_H
#define INSPECTABLETREEITEM_H
#include "TreeItem.h"


namespace SX
{
    namespace Instrument
    {
       class Experiment;
    }
}

//! Interface for tree Items for which
//! one wants to show internal properties
//! or interact.
class InspectableTreeItem : public TreeItem
{
public:
    InspectableTreeItem(Experiment* experiment);
    ~InspectableTreeItem();
    //! Tree items can expose a QWidget that show their internal properties.
    virtual QWidget* inspectItem() =0;
};

#endif // INSPECTABLETREEITEM_H
