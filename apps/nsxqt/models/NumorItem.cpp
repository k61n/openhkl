#include <string>

#include <QJsonArray>

#include <nsxlib/AABB.h>
#include <nsxlib/BoxMask.h>
#include <nsxlib/DataSet.h>
#include <nsxlib/Experiment.h>
#include <nsxlib/Logger.h>

#include "ExperimentItem.h"
#include "NumorItem.h"
#include "NumorPropertyWidget.h"

NumorItem::NumorItem(nsx::sptrDataSet data):
    InspectableTreeItem(),
    _data(data)
{
    setText("Numor");

    setEditable(false);

    setDragEnabled(false);
    setDropEnabled(false);

    setSelectable(false);

    setCheckable(true);
}

NumorItem::~NumorItem()
{
}

nsx::sptrDataSet NumorItem::data()
{
    return _data;
}

void NumorItem::exportHDF5(const std::string& filename) const
{
    if (filename.empty()) {
        return;
    }

    if (filename.compare(_data->filename())==0) {
        return;
    }

    try {
        _data->saveHDF5(filename);
    } catch(...) {
        nsx::error() << "The filename " << filename << " could not be saved. Maybe a permission problem.";
    }
}

QWidget* NumorItem::inspectItem()
{
    return new NumorPropertyWidget(this);
}

