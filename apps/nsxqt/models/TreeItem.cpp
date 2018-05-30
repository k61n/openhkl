#include <cassert>

#include <QStandardItem>
#include <QString>
#include <QJsonObject>
#include <QJsonArray>

#include "ExperimentItem.h"
#include "TreeItem.h"

TreeItem::TreeItem(): QStandardItem()
{
}

TreeItem::~TreeItem()
{
}

void TreeItem::setData(const QVariant &value, int role)
{
    #if 0
    if (role == Qt::EditRole)
    {
        QString newExptName = value.toString().trimmed();
        if (newExptName.isEmpty())
            return;
        _experiment->setName(newExptName.toStdString());
    }
    #endif
    QStandardItem::setData(value,role);
}

nsx::sptrExperiment TreeItem::experiment()
{
    return experimentItem().experiment();
}

ExperimentItem& TreeItem::experimentItem()
{
    ExperimentItem* exp_item = nullptr;
    QStandardItem* p = parent();

    while (p != nullptr) {
        exp_item = dynamic_cast<ExperimentItem*>(p);
        if (exp_item != nullptr) {
            break;
        }
        p = p->parent();
    }

    if (exp_item == nullptr) {
        throw std::runtime_error("TreeItem::importData(): no experiment in tree!");
    }

    return *exp_item;
}