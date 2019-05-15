//*************************************************************************************************
//
//! @file nsxgui/gui/models/experimentmodel.h
//! @brief defines classes ExperimentData, InstrumentModel, and ExperimentModel
//
//*************************************************************************************************

#ifndef EXPERIMENTMODEL_H
#define EXPERIMENTMODEL_H

#include <QList>

#include <build/core/include/core/Experiment.h>
#include <build/core/include/core/InstrumentTypes.h>

class ExperimentData {
public:
    ExperimentData() {}
    void findPeaks();
    void openInstrumentStates();
    void convertToHDF5();
    void removeSelectedData();
    void addData(nsx::sptrDataSet);
    QList<nsx::sptrDataSet> allData() { return data_; }
    nsx::sptrDataSet selectedData();
    void selectData(int);

private:
    QList<nsx::sptrDataSet> data_;
    int index_;
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
private:
	nsx::sptrExperiment experiment_;
    ExperimentData* data_;
    InstrumentModel* instrument_;
};

#endif //EXPERIMENTMODEL_H
