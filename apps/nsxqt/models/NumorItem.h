#ifndef NSXQT_NUMORITEM_H
#define NSXQT_NUMORITEM_H

#include <string>

#include <QJsonObject>

#include <nsxlib/DataTypes.h>
#include <nsxlib/InstrumentTypes.h>

#include "InspectableTreeItem.h"

class QWidget;

class NumorItem : public InspectableTreeItem
{
public:

    explicit NumorItem(nsx::sptrExperiment experiment, nsx::sptrDataSet data);
    ~NumorItem();
    QWidget* inspectItem() override;
    nsx::sptrDataSet getData();

    QJsonObject toJson() override;
    void fromJson(const QJsonObject& obj) override;
    void exportHDF5(const std::string& filename) const;

private:
    nsx::sptrDataSet _data;
};

#endif // NSXQT_NUMORITEM_H
