#include <string>

#include <QJsonArray>

#include <nsxlib/AABB.h>
#include <nsxlib/BoxMask.h>
#include <nsxlib/DataSet.h>
#include <nsxlib/Experiment.h>

#include "ExperimentItem.h"
#include "NumorItem.h"
#include "NumorPropertyWidget.h"

NumorItem::NumorItem(nsx::sptrDataSet data):
    InspectableTreeItem(),
    _data(data)
{
    setText("Numor");
    setEditable(false);
    setDragEnabled(true);
    setDropEnabled(true);
}

NumorItem::~NumorItem()
{
    experiment()->removeData(_data->filename());
}

nsx::sptrDataSet NumorItem::getData()
{
    return _data;
}

QJsonObject NumorItem::toJson()
{
    QJsonObject obj;
    QJsonArray masks;
    // todo(jonathan): reimplement this!
#if 0
    obj["filename"] = QString(getData()->filename().c_str());

    for (auto&& mask: getData()->getMasks()) {
        const Eigen::Vector3d lower(mask->lower());
        const Eigen::Vector3d upper(mask->upper());

        QJsonArray mask_obj;
        mask_obj.push_back(lower(0));
        mask_obj.push_back(lower(1));
        mask_obj.push_back(lower(2));
        mask_obj.push_back(upper(0));
        mask_obj.push_back(upper(1));
        mask_obj.   push_back(upper(2));

        masks.push_back(mask_obj);
    }

    obj["masks"] = masks;
#endif
    return obj;
}

void NumorItem::fromJson(const QJsonObject &obj)
{
  assert(obj["filename"].toString() == getData()->filename().c_str());
  QJsonArray masks = obj["masks"].toArray();

    for (auto&& mask: masks) {
        QJsonArray mask_arr = mask.toArray();

        Eigen::Vector3d lower, upper;

        assert(mask_arr.size() == 6);

        lower(0) = mask_arr[0].toDouble();
        lower(1) = mask_arr[1].toDouble();
        lower(2) = mask_arr[2].toDouble();

        upper(0) = mask_arr[3].toDouble();
        upper(1) = mask_arr[4].toDouble();
        upper(2) = mask_arr[5].toDouble();

        getData()->addMask(new nsx::BoxMask(nsx::AABB(lower, upper)));
    }
}

void NumorItem::exportHDF5(const std::string& filename) const
{
    if (filename.empty()) {
        return;
    }
    _data->saveHDF5(filename);
}

QWidget* NumorItem::inspectItem()
{
    return new NumorPropertyWidget(this);
}

