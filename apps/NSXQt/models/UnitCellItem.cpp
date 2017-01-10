#include "Diffractometer.h"
#include "Experiment.h"
#include "Sample.h"
#include "UnitCell.h"

#include "models/UnitCellItem.h"
#include "tree/UnitCellPropertyWidget.h"

UnitCellItem::UnitCellItem(std::shared_ptr<SX::Instrument::Experiment> experiment,std::shared_ptr<SX::Crystal::UnitCell> cell):
    InspectableTreeItem(experiment),
    _cell(cell)
{
    QIcon icon(":/resources/unitCellIcon.png");
    setIcon(icon);
    setEditable(true);
    setDragEnabled(false);
    setDropEnabled(false);
    setForeground(QBrush(Qt::red));
}

UnitCellItem::~UnitCellItem()
{
    _experiment->getDiffractometer()->getSample()->removeUnitCell(_cell);
}

QWidget* UnitCellItem::inspectItem()
{
    return new UnitCellPropertyWidget(this);
}


std::shared_ptr<SX::Crystal::UnitCell> UnitCellItem::getUnitCell()
{
    return _cell;
}
