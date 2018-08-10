#pragma once

#include <string>

#include <QJsonObject>

#include <nsxlib/DataTypes.h>
#include <nsxlib/InstrumentTypes.h>

#include "InspectableTreeItem.h"

class QWidget;

class NumorItem : public InspectableTreeItem
{
public:

    explicit NumorItem(nsx::sptrDataSet data);
    ~NumorItem();
    QWidget* inspectItem() override;
    nsx::sptrDataSet data();
    void exportHDF5(const std::string& filename) const;

private:
    nsx::sptrDataSet _data;
};
