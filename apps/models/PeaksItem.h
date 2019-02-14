#pragma once

#include <core/DataTypes.h>
#include <core/InstrumentTypes.h>

#include "InspectableTreeItem.h"

class PeakListItem;

class PeaksItem : public InspectableTreeItem {

public:
  explicit PeaksItem();

  virtual QWidget *inspectItem() override;

  nsx::PeakList allPeaks();

  nsx::PeakList selectedPeaks();

  void openAutoIndexingFrame();

  void openPeakFilterDialog();

  void removeUnitCell(nsx::sptrUnitCell unit_cell);

  void integratePeaks();

  void findSpaceGroup();

  void showPeaksOpenGL();

  void absorptionCorrection();

  void buildShapeLibrary();

  void refine();

  void autoAssignUnitCell();

  void openUserDefinedUnitCellIndexerFrame();

  void normalizeToMonitor();
};
