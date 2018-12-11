#pragma once

#include "TreeItem.h"

//! Interface for tree Items for which
//! one wants to show internal properties
//! or interact.
class InspectableTreeItem : public TreeItem
{
public:
    InspectableTreeItem();
    ~InspectableTreeItem();
    //! Tree items can expose a QWidget that show their internal properties.
    virtual QWidget* inspectItem() = 0;
};
