#include <nsxlib/Diffractometer.h>
#include <nsxlib/Experiment.h>
#include <nsxlib/Logger.h>
#include <nsxlib/Sample.h>
#include <nsxlib/UnitCell.h>

#include "DialogTransformationMatrix.h"
#include "DialogUnitCellParameters.h"
#include "UnitCellItem.h"
#include "UnitCellPropertyWidget.h"

UnitCellItem::UnitCellItem(nsx::sptrExperiment experiment, nsx::sptrUnitCell cell):
    InspectableTreeItem(experiment),
    _cell(cell)
{
    QIcon icon(":/resources/unitCellIcon.png");
    setText(QString::fromStdString(_cell->getName()));
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

nsx::sptrUnitCell UnitCellItem::unitCell()
{
    return _cell;
}

void UnitCellItem::info() const
{
    nsx::debug() << "" << *_cell;
}

void UnitCellItem::openChangeUnitCellDialog()
{
    DialogUnitCellParameters* dialog = new DialogUnitCellParameters(_cell);
    dialog->exec();
}

void UnitCellItem::openTransformationMatrixDialog()
{
    DialogTransformationmatrix* dialog=new DialogTransformationmatrix(_cell);
    dialog->exec();
}
