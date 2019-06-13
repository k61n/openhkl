//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/models/ExperimentModel.h
//! @brief     Defines classes ExperimentData, UnitCellsModel, InstrumentModel, and ExperimentModel
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef GUI_MODELS_EXPERIMENTMODEL_H
#define GUI_MODELS_EXPERIMENTMODEL_H

#include "tables/crystal/UnitCell.h"
#include "core/experiment/Experiment.h"
#include "core/instrument/InstrumentTypes.h"
#include "gui/models/PeakLists.h"

class ExperimentData {
public:
    ExperimentData() {}
    void convertToHDF5();
    void removeSelectedData();
    void addData(nsx::sptrDataSet);
    QList<nsx::sptrDataSet> allData() { return data_; }
    nsx::DataList allDataVector();
    nsx::sptrDataSet selectedData();
    void selectData(int);

private:
    QList<nsx::sptrDataSet> data_;
    int index_;
};

class UnitCellsModel {
public:
    UnitCellsModel();

    void appendUnitCell(nsx::sptrUnitCell cell) { unitCells_.append(cell); }
    nsx::sptrUnitCell selectedCell();
    QList<nsx::sptrUnitCell> allUnitCells() { return unitCells_; }

private:
    QList<nsx::sptrUnitCell> unitCells_;
    int selected_;
};

class InstrumentModel {
public:
    InstrumentModel(const QString&, const QString&);

    void detectorGlobalOffsets();
    void sampleGlobalOffsets();
    void openIsotopesDatabase();
    const QString& instrumentName() { return instrumentName_; }
    const QString& sourceName() { return sourceName_; }

private:
    QString instrumentName_;
    QString sourceName_;
};

class ExperimentModel {
public:
    ExperimentModel(nsx::sptrExperiment);
    ~ExperimentModel();
    nsx::sptrExperiment experiment() { return experiment_; }
    ExperimentData* data() { return data_; }
    InstrumentModel* instrument() { return instrument_; }
    void addData(nsx::sptrDataSet data) { data_->addData(data); }
    PeaksModel* peaks() { return peaks_; }
    UnitCellsModel* unitCells() { return units_; }
    void removeSelectedData();

private:
    nsx::sptrExperiment experiment_;
    ExperimentData* data_;
    InstrumentModel* instrument_;
    PeaksModel* peaks_;
    UnitCellsModel* units_;
};

#endif // GUI_MODELS_EXPERIMENTMODEL_H
