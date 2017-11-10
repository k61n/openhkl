#include <cassert>

#include <QStandardItem>
#include <QString>
#include <QJsonObject>
#include <QJsonArray>

#include "TreeItem.h"

TreeItem::TreeItem(nsx::sptrExperiment experiment) : QStandardItem(), _experiment(experiment)
{
}

TreeItem::~TreeItem()
{
}

void TreeItem::setData(const QVariant &value, int role)
{
    if (role == Qt::EditRole)
    {
        QString newExptName = value.toString().trimmed();
        if (newExptName.isEmpty())
            return;
        _experiment->setName(newExptName.toStdString());
    }
    QStandardItem::setData(value,role);
}

QJsonObject TreeItem::toJson()
{
    QJsonObject obj;
    QJsonArray children;

    obj["name"] = text();

    for (int i = 0; i < rowCount(); ++i) {
        TreeItem* tree_item = dynamic_cast<TreeItem*>(this->child(i));
        assert(tree_item != nullptr);
        children.push_back(tree_item->toJson());
    }

    obj["data"] = children;

    return obj;
}

void TreeItem::fromJson(const QJsonObject &obj)
{
    QString name = obj["name"].toString();
    QJsonArray data = obj["data"].toArray();
}

nsx::sptrExperiment TreeItem::getExperiment()
{
    return _experiment;
}
