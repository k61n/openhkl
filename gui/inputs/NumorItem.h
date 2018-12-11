#pragma once

#include <string>

#include <QJsonObject>

#include <core/DataTypes.h>
#include "InspectableTreeItem.h"

class QWidget;

class NumorItem : public InspectableTreeItem
{
public:
    NumorItem() = delete;
    explicit NumorItem(nsx::sptrDataSet data);

    virtual QVariant data(int role) const override;

    QWidget* inspectItem() override;

    void exportHDF5(const std::string& filename) const;

private:
    nsx::sptrDataSet _data;
};
