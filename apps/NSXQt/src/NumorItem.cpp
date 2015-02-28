#include "NumorItem.h"
#include "Experiment.h"
#include "NumorPropertyWidget.h"
#include "IData.h"

NumorItem::NumorItem(SX::Instrument::Experiment* experiment,SX::Data::IData* data) :
    InspectableTreeItem(experiment),
    _data(data)
{
    setText("Numor");
    setEditable(false);
    setDragEnabled(true);
    setDropEnabled(true);
}

SX::Data::IData* NumorItem::getData()
{
    return _data;
}

QWidget* NumorItem::inspectItem()
{
    return new NumorPropertyWidget(this);
}

