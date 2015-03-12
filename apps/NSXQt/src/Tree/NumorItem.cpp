#include "Experiment.h"
#include "IData.h"

#include "Tree/NumorPropertyWidget.h"
#include "Tree/NumorItem.h"

NumorItem::NumorItem(SX::Instrument::Experiment* experiment,SX::Data::IData* data) :
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
   _experiment->removeData(_data->getBasename());
}

SX::Data::IData* NumorItem::getData()
{
    return _data;
}

QWidget* NumorItem::inspectItem()
{
    return new NumorPropertyWidget(this);
}

