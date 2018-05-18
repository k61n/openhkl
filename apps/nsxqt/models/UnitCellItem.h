#ifndef NSXQT_UNITCELLITEM_H
#define NSXQT_UNITCELLITEM_H

#include "InspectableTreeItem.h"

#include <nsxlib/CrystalTypes.h>
#include <nsxlib/InstrumentTypes.h>

class QWidget;

class UnitCellItem : public InspectableTreeItem
{
public:
    UnitCellItem(nsx::sptrUnitCell);
    ~UnitCellItem();
    QWidget* inspectItem();
    nsx::sptrUnitCell unitCell();

    void info() const;

    void openTransformationMatrixDialog();
    void openChangeUnitCellDialog();

private:
    nsx::sptrUnitCell _cell;
};

#endif // NSXQT_UNITCELLITEM_H
