#include "UnitCellItem.h"
#include "Experiment.h"
#include "UnitCell.h"
#include "UnitCellPropertyWidget.h"
#include "Diffractometer.h"
#include "Sample.h"

UnitCellItem::UnitCellItem(SX::Instrument::Experiment* experiment,std::shared_ptr<SX::Crystal::UnitCell> cell):
    InspectableTreeItem(experiment),
    _cell(cell)
{
    QIcon icon(":/resources/unitCellIcon.png");
    setIcon(icon);
    setEditable(true);
    setDragEnabled(false);
    setDropEnabled(false);
}

UnitCellItem::~UnitCellItem()
{
    _experiment->getDiffractometer()->getSample()->removeUnitCell(_cell);
}

QWidget* UnitCellItem::inspectItem()
{
    return new UnitCellPropertyWidget(this);
}


std::shared_ptr<SX::Crystal::UnitCell> UnitCellItem::getCell()
{
    return _cell;
}
