#include "TreeItem.h"

#include "ExperimentItem.h"

#include <QStandardItem>


TreeItem::TreeItem() : QStandardItem()
{
}

void TreeItem::setData(const QVariant &value, int role)
{
    QStandardItem::setData(value,role);
}

nsx::sptrExperiment TreeItem::experiment()
{
    return experimentItem()->experiment();
}

ExperimentItem* TreeItem::experimentItem() const
{
    ExperimentItem* exp_item = nullptr;
    QStandardItem* p = parent();

    while (p != nullptr) {
        exp_item = dynamic_cast<ExperimentItem*>(p);
        if (exp_item != nullptr)
            break;
        p = p->parent();
    }

    if (exp_item == nullptr)
        throw std::runtime_error("TreeItem: no experiment in tree!");

    return exp_item;
}

SessionModel* TreeItem::model() const
{
    return dynamic_cast<SessionModel*>(QStandardItem::model());
}
