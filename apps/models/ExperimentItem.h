//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      apps/models/ExperimentItem.h
//! @brief     Defines ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#pragma once

#include <memory>
#include <string>

#include <QJsonObject>
#include <QStandardItemModel>

#include <build/core/include/core/Experiment.h>
#include <build/core/include/core/InstrumentTypes.h>

#include "apps/models/TreeItem.h"

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

    InstrumentItem* instrumentItem();

    PeaksItem* peaksItem();

    DataItem* dataItem();

    UnitCellsItem* unitCellsItem();

    LibraryItem* libraryItem();

    void writeLogFiles();

private:
    nsx::sptrExperiment _experiment;

    InstrumentItem* _instr;

    DataItem* _data;

    UnitCellsItem* _unitCells;

    PeaksItem* _peaks;

    LibraryItem* _library;
};
