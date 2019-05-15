
#include "nsxgui/gui/models/experimentmodel.h"
#include "nsxgui/qcr/engine/mixin.h"
//  ***********************************************************************************************
//! @class ExperimentData

void ExperimentData::addData(nsx::sptrDataSet data)
{
    data_.append(data);
    if ( (index_<0) || (index_>=data_.size()) )
        index_ = 0;
    gRoot->remakeAll();
}

void ExperimentData::removeSelectedData()
{
    if (data_.size() > 0)
        data_.removeAt(index_);
    if (index_ >= data_.size())
        index_--;
    gRoot->remakeAll();
}

void ExperimentData::selectData(int i)
{
    if ( i>=0 && i<data_.size())
        index_ = i;
}

nsx::sptrDataSet ExperimentData::selectedData()
{
    if (data_.size()>0 && index_<data_.size() && index_>=0)
        return data_.at(index_);
    return nullptr;
}

void ExperimentData::findPeaks()
{
    //TODO: implement the function
}

void ExperimentData::openInstrumentStates()
{
    //TODO: implement the function
}

void ExperimentData::convertToHDF5()
{
    //TODO: implement the function
}

//  ***********************************************************************************************
//! @class InstrumentModel

InstrumentModel::InstrumentModel(const QString& name, const QString& sourceName)
{
    //TODO: implement constructor
}





//  ***********************************************************************************************
//! @class ExperimentModel

ExperimentModel::ExperimentModel(nsx::sptrExperiment experiment)
	: experiment_{experiment}
{
    data_ = new ExperimentData;
    instrument_ = new InstrumentModel(QString::fromStdString(experiment_->name()), "sourceName");
}
