#ifndef UNITCELLITEM_H
#define UNITCELLITEM_H
#include "InspectableTreeItem.h"
#include <memory>

namespace SX{
    namespace Instrument{
        class Experiment;
    }
    namespace Crystal{
        class UnitCell;
    }
}

class QWidget;

class UnitCellItem : public InspectableTreeItem
{
public:
    UnitCellItem(SX::Instrument::Experiment* experiment,std::shared_ptr<SX::Crystal::UnitCell>);
    ~UnitCellItem();
    QWidget* inspectItem();
    std::shared_ptr<SX::Crystal::UnitCell> getCell();
private:
    std::shared_ptr<SX::Crystal::UnitCell> _cell;
};

#endif // UNITCELLITEM_H
