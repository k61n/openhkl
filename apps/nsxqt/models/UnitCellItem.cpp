#include <nsxlib/Diffractometer.h>
#include <nsxlib/Experiment.h>
#include <nsxlib/Logger.h>
#include <nsxlib/Sample.h>
#include <nsxlib/UnitCell.h>

#include "DialogSpaceGroup.h"
#include "DialogTransformationMatrix.h"
#include "DialogUnitCellParameters.h"
#include "ExperimentItem.h"
#include "UnitCellItem.h"
#include "UnitCellPropertyWidget.h"

UnitCellItem::UnitCellItem(nsx::sptrUnitCell cell):
    InspectableTreeItem(),
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
    experiment()->diffractometer()->getSample()->removeUnitCell(_cell);
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

void UnitCellItem::determineSpaceGroup()
{
    auto selected_peaks = experimentItem().peaks().selectedPeaks();
    // todo
    DialogSpaceGroup dlg(selected_peaks);

    if (!dlg.exec()) {
        return;
    }

    auto group = dlg.getSelectedGroup();
    _cell->setSpaceGroup(group);
}
