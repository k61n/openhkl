#include <nsxlib/instrument/Experiment.h>
#include <nsxlib/data/IData.h>

#include "tree/NumorPropertyWidget.h"
#include "models/NumorItem.h"

#include <iostream>
#include <QJsonArray>

using std::cout;
using std::endl;
using SX::Geometry::AABB;

NumorItem::NumorItem(std::shared_ptr<SX::Instrument::Experiment> experiment,std::shared_ptr<SX::Data::DataSet> data) :
    InspectableTreeItem(experiment),
    _data(data)
{
    setText("Numor");
    setEditable(false);
    setDragEnabled(true);
    setDropEnabled(true);
}

NumorItem::~NumorItem()
{
    cout << "NumorItem destructor: removing data " << _data->getBasename() << endl;
   _experiment->removeData(_data->getBasename());
   //delete _data; // JMF fix memory leak detected by valgrind
}

std::shared_ptr<SX::Data::DataSet> NumorItem::getData()
{
    return _data;
}

QJsonObject NumorItem::toJson()
{
    QJsonObject obj;
    QJsonArray masks;

    obj["filename"] = QString(getData()->getFilename().c_str());

    for (auto&& mask: getData()->getMasks()) {
        const Eigen::Vector3d lower(mask->getLower());
        const Eigen::Vector3d upper(mask->getUpper());

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
    return obj;
}

void NumorItem::fromJson(const QJsonObject &obj)
{
  assert(obj["filename"].toString() == getData()->getFilename().c_str());
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

        getData()->addMask(new AABB<double, 3>(lower, upper));
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

