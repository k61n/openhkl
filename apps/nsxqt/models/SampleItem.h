#ifndef NSXQT_SAMPLEITEM_H
#define NSXQT_SAMPLEITEM_H

#include <QJsonObject>
#include <QList>

#include <nsxlib/InstrumentTypes.h>

#include "InspectableTreeItem.h"

class UnitCellItem;

class SampleItem : public InspectableTreeItem
{
public:
    explicit SampleItem(nsx::sptrExperiment experiment);
    void setData(const QVariant& value, int role = Qt::UserRole + 1) override;
    QWidget* inspectItem() override;
    QJsonObject toJson() override;
    void fromJson(const QJsonObject& obj) override;

    QList<UnitCellItem*> getUnitCellItems();

    void addUnitCell();

};

#endif // NSXQT_SAMPLEITEM_H
