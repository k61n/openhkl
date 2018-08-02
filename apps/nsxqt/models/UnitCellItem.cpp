#include <memory>

#include <QInputDialog>

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

UnitCellItem::UnitCellItem(nsx::sptrUnitCell unit_cell)
: InspectableTreeItem(),
  _unit_cell(unit_cell)
{
    setText(QString::fromStdString(_unit_cell->name()));

    setIcon(QIcon(":/resources/unitCellIcon.png"));

    setEditable(true);
    setDragEnabled(false);
    setDropEnabled(false);
    setForeground(QBrush(Qt::red));
}

UnitCellItem::~UnitCellItem()
{
    nsx::UnitCellList& unit_cells = experimentItem()->experiment()->diffractometer()->sample()->unitCells();
    auto it = std::find(unit_cells.begin(),unit_cells.end(),_unit_cell);
    if (it != unit_cells.end()) {
        unit_cells.erase(it);
    }
}

QVariant UnitCellItem::data(int role) const
{
    switch(role) {
    case(Qt::DecorationRole):
        return QIcon(":/resources/unitCellIcon.png");
    case(Qt::EditRole):
        return QString::fromStdString(_unit_cell->name());
    case(Qt::DisplayRole):
        return QString::fromStdString(_unit_cell->name());
    case(Qt::UserRole):
        return QVariant::fromValue(_unit_cell);
    }
    return InspectableTreeItem::data(role);
}

void UnitCellItem::setData(const QVariant& value, int role)
{
    if (!index().isValid()) {
        return;
    }

    switch(role) {
    case (Qt::DisplayRole):
        _unit_cell->setName(value.toString().toStdString());
        break;
    case (Qt::EditRole):
        _unit_cell->setName(value.toString().toStdString());
        break;
    }

    InspectableTreeItem::setData(value,role);
}

QWidget* UnitCellItem::inspectItem()
{
    return new UnitCellPropertyWidget(this);
}

nsx::sptrUnitCell UnitCellItem::unitCell()
{
    return _unit_cell;
}

void UnitCellItem::info() const
{
    nsx::debug() << "" << *(_unit_cell);
}

void UnitCellItem::openChangeUnitCellDialog()
{
    std::unique_ptr<DialogUnitCellParameters> dialog(new DialogUnitCellParameters(_unit_cell));
    dialog->exec();

    emitDataChanged();
}

void UnitCellItem::openTransformationMatrixDialog()
{
    std::unique_ptr<DialogTransformationmatrix> dialog(new DialogTransformationmatrix(_unit_cell));
    dialog->exec();

    emitDataChanged();
}

void UnitCellItem::openSpaceGroupDialog()
{
    auto selected_peaks = experimentItem()->peaksItem()->selectedPeaks();

    std::unique_ptr<DialogSpaceGroup> dialog(new DialogSpaceGroup(selected_peaks));

    if (!dialog->exec()) {
        return;
    }

    auto group = dialog->getSelectedGroup();
    _unit_cell->setSpaceGroup(group);

    emitDataChanged();
}

void UnitCellItem::openIndexingToleranceDialog()
{
    bool ok;
    double tolerance = QInputDialog::getDouble(nullptr, "Enter indexing tolerance","",_unit_cell->indexingTolerance(), 0.0, 1.0, 6, &ok);
    if (ok) {
        _unit_cell->setIndexingTolerance(tolerance);
    }

    emitDataChanged();
}
