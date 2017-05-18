#ifndef NSXQT_NUMORITEM_H
#define NSXQT_NUMORITEM_H

#include <string>

#include <QJsonObject>

#include "InspectableTreeItem.h"

namespace nsx
{
class DataSet;
class Experiment;
}

class QWidget;

class NumorItem : public InspectableTreeItem
{
public:
    explicit NumorItem(std::shared_ptr<Experiment> experiment, std::shared_ptr<nsx::DataSet> data);
    ~NumorItem();
    QWidget* inspectItem() override;
    std::shared_ptr<nsx::DataSet> getData();

    QJsonObject toJson() override;
    void fromJson(const QJsonObject& obj) override;
    void exportHDF5(const std::string& filename) const;

private:
    std::shared_ptr<nsx::DataSet> _data;
};

#endif // NSXQT_NUMORITEM_H
