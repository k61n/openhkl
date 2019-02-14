#pragma once

#include <memory>
#include <string>

#include <QJsonObject>
#include <QStandardItemModel>

#include <core/Experiment.h>
#include <core/InstrumentTypes.h>

#include "TreeItem.h"

class DataItem;
class LibraryItem;
class InstrumentItem;
class PeaksItem;
class UnitCellsItem;

class ExperimentItem : public TreeItem {
public:
  explicit ExperimentItem(nsx::sptrExperiment experiment);

  virtual ~ExperimentItem();

  nsx::sptrExperiment experiment() { return _experiment; }

  InstrumentItem *instrumentItem();

  PeaksItem *peaksItem();

  DataItem *dataItem();

  UnitCellsItem *unitCellsItem();

  LibraryItem *libraryItem();

  void writeLogFiles();

private:
  nsx::sptrExperiment _experiment;

  InstrumentItem *_instr;

  DataItem *_data;

  UnitCellsItem *_unitCells;

  PeaksItem *_peaks;

  LibraryItem *_library;
};
