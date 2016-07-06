#include "Experiment.h"
#include "IData.h"

#include "Tree/NumorPropertyWidget.h"
#include "Tree/NumorItem.h"

#include <iostream>

using std::cout;
using std::endl;

NumorItem::NumorItem(SX::Instrument::Experiment* experiment,std::shared_ptr<SX::Data::IData> data) :
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

std::shared_ptr<SX::Data::IData> NumorItem::getData()
{
    return _data;
}

QWidget* NumorItem::inspectItem()
{
    return new NumorPropertyWidget(this);
}

