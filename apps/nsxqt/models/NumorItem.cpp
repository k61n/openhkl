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

