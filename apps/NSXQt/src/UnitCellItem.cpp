#include "UnitCellItem.h"
#include "Experiment.h"
#include "UnitCell.h"
#include "UnitCellPropertyWidget.h"

UnitCellItem::UnitCellItem(SX::Instrument::Experiment* experiment,std::shared_ptr<SX::Crystal::UnitCell> cell):
    InspectableTreeItem(experiment),
    _cell(cell)
{
    QIcon icon(":/resources/unitCellIcon.png");
    setIcon(icon);
    setEditable(false);
    setDragEnabled(false);
    setDropEnabled(false);
}

QWidget* UnitCellItem::inspectItem()
{
    return new UnitCellPropertyWidget(this);
}


std::shared_ptr<SX::Crystal::UnitCell> UnitCellItem::getCell()
{
    return _cell;
}
