#ifndef UNITCELLITEM_H
#define UNITCELLITEM_H

#include <memory>

#include "InspectableTreeItem.h"

#include <nsxlib/utils/Types.h>

namespace nsx
{
class Experiment;
class UnitCell;
}

class QWidget;

class UnitCellItem : public InspectableTreeItem
{
public:
    UnitCellItem(std::shared_ptr<nsx::Experiment> experiment,std::shared_ptr<nsx::UnitCell>);
    ~UnitCellItem();
    QWidget* inspectItem();
    std::shared_ptr<nsx::UnitCell> getUnitCell();

    void info() const;

    void openTransformationMatrixDialog();
    void openChangeUnitCellDialog();

private:
    std::shared_ptr<nsx::UnitCell> _cell;
};

#endif // UNITCELLITEM_H
