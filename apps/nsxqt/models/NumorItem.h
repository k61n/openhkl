#ifndef NUMORITEM_H
#define NUMORITEM_H

#include <string>
#include <memory>
#include "InspectableTreeItem.h"

class QWidget;
namespace SX
{
    namespace Instrument
    {
       class Experiment;
    }
    namespace Data{
        class IData;
    }
}

class NumorItem : public InspectableTreeItem
{
public:
    explicit NumorItem(std::shared_ptr<Experiment> experiment, std::shared_ptr<SX::Data::IData> data);
    ~NumorItem();
    QWidget* inspectItem() override;
    std::shared_ptr<SX::Data::IData> getData();

    QJsonObject toJson() override;
    void fromJson(const QJsonObject& obj) override;
    void exportHDF5(const std::string& filename) const;

private:
    std::shared_ptr<SX::Data::IData> _data;
};

#endif // NUMORITEM_H
