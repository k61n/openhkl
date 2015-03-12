#ifndef DATAITEM_H
#define DATAITEM_H
#include <string>
#include "TreeItem.h"

namespace SX
{
    namespace Instrument
    {
       class Experiment;
    }
}

class DataItem : public TreeItem
{
public:
    explicit DataItem(Experiment* experiment);
};

#endif // DATAITEM_H
