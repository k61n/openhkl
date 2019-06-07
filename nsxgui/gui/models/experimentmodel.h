//*************************************************************************************************
//
//! @file nsxgui/gui/models/experimentmodel.h
//! @brief defines classes ExperimentData, InstrumentModel, and ExperimentModel
//
//*************************************************************************************************

#ifndef EXPERIMENTMODEL_H
#define EXPERIMENTMODEL_H

#include <QList>
#include <vector>

#include <build/core/include/core/CrystalTypes.h>
#include <build/core/include/core/Experiment.h>
#include <build/core/include/core/InstrumentTypes.h>
#include <build/core/include/core/UnitCell.h>

#include "nsxgui/gui/models/peaklists.h"

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

private:
    nsx::sptrExperiment experiment_;
    ExperimentData* data_;
    InstrumentModel* instrument_;
    PeaksModel* peaks_;
    UnitCellsModel* units_;
};

#endif // EXPERIMENTMODEL_H
