#pragma once

#include "InspectableTreeItem.h"

#include <core/CrystalTypes.h>
#include <core/InstrumentTypes.h>

class QWidget;

class UnitCellItem : public InspectableTreeItem {
public:
  UnitCellItem(nsx::sptrUnitCell unit_cell);
  ~UnitCellItem();

  virtual QVariant data(int role) const override;
  virtual void setData(const QVariant &value, int role) override;

  QWidget *inspectItem();

  void info() const;

public slots:
  void openIndexingToleranceDialog();
  void openTransformationMatrixDialog();
  void openChangeUnitCellDialog();

  void openSpaceGroupDialog();

private:
  nsx::sptrUnitCell _unit_cell;
};
