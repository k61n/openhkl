#ifndef SAMPLEITEM_H
#define SAMPLEITEM_H

#include <string>
#include <memory>

#include <QList>

#include "InspectableTreeItem.h"

class UnitCellItem;

using namespace SX::Instrument;

class SampleItem : public InspectableTreeItem
{
public:
    explicit SampleItem(std::shared_ptr<Experiment> experiment);
    void setData(const QVariant& value, int role = Qt::UserRole + 1) override;
    QWidget* inspectItem() override;
    QJsonObject toJson() override;
    void fromJson(const QJsonObject& obj) override;

    QList<UnitCellItem*> getUnitCellItems();

    void addUnitCell();

};

#endif // SAMPLEITEM_H
