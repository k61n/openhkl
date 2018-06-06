#include <nsxlib/Diffractometer.h>
#include <nsxlib/Experiment.h>
#include <nsxlib/Logger.h>
#include <nsxlib/Sample.h>
#include <nsxlib/UnitCell.h>

#include "DialogSpaceGroup.h"
#include "DialogTransformationMatrix.h"
#include "DialogUnitCellParameters.h"
#include "ExperimentItem.h"
#include "MetaTypes.h"
#include "PeaksItem.h"
#include "UnitCellItem.h"
#include "UnitCellPropertyWidget.h"

UnitCellItem::UnitCellItem(nsx::sptrUnitCell cell):
    InspectableTreeItem(),
    _cell(cell)
{
    setEditable(true);
    setDragEnabled(false);
    setDropEnabled(false);
    setForeground(QBrush(Qt::red));
}

UnitCellItem::~UnitCellItem()
{
    experimentItem()->experiment()->diffractometer()->getSample()->removeUnitCell(_cell);
}

void UnitCellItem::setData(const QVariant& value, int role)
{
    if (!index().isValid()) {
        return;
    }

    switch(role) {
    case (Qt::DisplayRole):
        _cell->setName(value.toString().toStdString());
        break;
    case (Qt::EditRole):
        _cell->setName(value.toString().toStdString());
        break;
    case(Qt::UserRole):
        _cell = value.value<nsx::sptrUnitCell>();
        break;
    }

    InspectableTreeItem::setData(value,role);
}

QVariant UnitCellItem::data(int role) const
{
    switch(role) {
    case(Qt::DecorationRole):
        return QIcon(":/resources/unitCellIcon.png");
    case(Qt::EditRole):
        return QString::fromStdString(_cell->name());
    case(Qt::DisplayRole):
        return QString::fromStdString(_cell->name());
    case(Qt::UserRole):
        return QVariant::fromValue(_cell);
    }
    return QVariant::Invalid;
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
    auto selected_peaks = experimentItem()->peaksItem()->selectedPeaks();
    // todo
    DialogSpaceGroup dlg(selected_peaks);

    if (!dlg.exec()) {
        return;
    }

    auto group = dlg.getSelectedGroup();
    _cell->setSpaceGroup(group);
}
