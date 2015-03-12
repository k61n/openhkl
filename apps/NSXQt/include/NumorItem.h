#ifndef NUMORITEM_H
#define NUMORITEM_H

#include <string>
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
    explicit NumorItem(Experiment* experiment,SX::Data::IData* data);
    ~NumorItem();
    QWidget* inspectItem();
    SX::Data::IData* getData();
private:
    SX::Data::IData* _data;
};

#endif // NUMORITEM_H
