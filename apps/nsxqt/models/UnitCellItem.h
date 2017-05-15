#ifndef UNITCELLITEM_H
#define UNITCELLITEM_H

#include "InspectableTreeItem.h"

#include <memory>

#include <nsxlib/utils/Types.h>

namespace nsx
{
namespace Instrument
{
class Experiment;
}
namespace Crystal{
class UnitCell;
}
}

class QWidget;

using nsx::Crystal::sptrUnitCell;

class UnitCellItem : public InspectableTreeItem
{
public:
    UnitCellItem(std::shared_ptr<nsx::Instrument::Experiment> experiment,std::shared_ptr<nsx::Crystal::UnitCell>);
    ~UnitCellItem();
    QWidget* inspectItem();
    std::shared_ptr<nsx::Crystal::UnitCell> getUnitCell();

    void info() const;

    void openTransformationMatrixDialog();
    void openChangeUnitCellDialog();

private:
    std::shared_ptr<nsx::Crystal::UnitCell> _cell;
};

#endif // UNITCELLITEM_H
