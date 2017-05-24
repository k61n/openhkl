#ifndef NSXQT_UNITCELLITEM_H
#define NSXQT_UNITCELLITEM_H

#include "InspectableTreeItem.h"

#include <nsxlib/crystal/CrystalTypes.h>
#include <nsxlib/instrument/InstrumentTypes.h>

class QWidget;

class UnitCellItem : public InspectableTreeItem
{
public:
    UnitCellItem(nsx::sptrExperiment experiment, nsx::sptrUnitCell);
    ~UnitCellItem();
    QWidget* inspectItem();
    nsx::sptrUnitCell getUnitCell();

    void info() const;

    void openTransformationMatrixDialog();
    void openChangeUnitCellDialog();

private:
    nsx::sptrUnitCell _cell;
};

#endif // NSXQT_UNITCELLITEM_H
