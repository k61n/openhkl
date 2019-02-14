#include <memory>

#include <QInputDialog>

#include <core/Diffractometer.h>
#include <core/Experiment.h>
#include <core/Logger.h>
#include <core/Peak3D.h>
#include <core/Sample.h>
#include <core/UnitCell.h>

#include "DialogSpaceGroup.h"
#include "DialogTransformationMatrix.h"
#include "DialogUnitCellParameters.h"
#include "ExperimentItem.h"
#include "MetaTypes.h"
#include "PeaksItem.h"
#include "UnitCellItem.h"
#include "UnitCellPropertyWidget.h"
#include "UnitCellsItem.h"

UnitCellItem::UnitCellItem(nsx::sptrUnitCell unit_cell)
    : InspectableTreeItem(), _unit_cell(unit_cell) {
  setText(QString::fromStdString(_unit_cell->name()));

  setIcon(QIcon(":/resources/unitCellIcon.png"));

  setEditable(true);

  setDragEnabled(false);
  setDropEnabled(false);

  setSelectable(false);

  setCheckable(true);

  setCheckState(Qt::Unchecked);
}

UnitCellItem::~UnitCellItem() {}

QVariant UnitCellItem::data(int role) const {
  switch (role) {
  case (Qt::DecorationRole): {
    return QIcon(":/resources/unitCellIcon.png");
  }
  case (Qt::EditRole): {
    return QString::fromStdString(_unit_cell->name());
  }
  case (Qt::DisplayRole): {
    return QString::fromStdString(_unit_cell->name());
  }
  case (Qt::UserRole): {
    return QVariant::fromValue(_unit_cell);
  }
  }
  return InspectableTreeItem::data(role);
}

void UnitCellItem::setData(const QVariant &value, int role) {
  switch (role) {
  case (Qt::DisplayRole): {
    _unit_cell->setName(value.toString().toStdString());
    break;
  }
  case (Qt::EditRole): {
    _unit_cell->setName(value.toString().toStdString());
    break;
  }
  }
  InspectableTreeItem::setData(value, role);
}

QWidget *UnitCellItem::inspectItem() {
  return new UnitCellPropertyWidget(this);
}

void UnitCellItem::info() const { nsx::debug() << "" << *(_unit_cell); }

void UnitCellItem::openChangeUnitCellDialog() {
  std::unique_ptr<DialogUnitCellParameters> dialog(
      new DialogUnitCellParameters(_unit_cell));
  dialog->exec();

  emitDataChanged();
}

void UnitCellItem::openTransformationMatrixDialog() {
  std::unique_ptr<DialogTransformationMatrix> dialog(
      new DialogTransformationMatrix(_unit_cell));
  dialog->exec();

  emitDataChanged();
}

void UnitCellItem::openSpaceGroupDialog() {
  auto selected_peaks = experimentItem()->peaksItem()->selectedPeaks();

  std::unique_ptr<DialogSpaceGroup> dialog(
      new DialogSpaceGroup(selected_peaks));

  if (!dialog->exec()) {
    return;
  }

  auto group = dialog->getSelectedGroup();

  auto space_group = nsx::SpaceGroup(group);

  _unit_cell->setSpaceGroup(space_group);

  emitDataChanged();
}

void UnitCellItem::openIndexingToleranceDialog() {
  bool ok;
  double tolerance = QInputDialog::getDouble(
      nullptr, "Enter indexing tolerance", "", _unit_cell->indexingTolerance(),
      0.0, 1.0, 6, &ok);
  if (ok) {
    _unit_cell->setIndexingTolerance(tolerance);
  }

  emitDataChanged();
}
