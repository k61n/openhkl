//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/models/experimentmodel.cpp
//! @brief     Implements ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************


#include "gui/models/experimentmodel.h"
#include "gui/models/session.h"
#include <QCR/engine/logger.h>
#include <QCR/engine/mixin.h>
#include "core/peak/Peak3D.h"
//  ***********************************************************************************************
//! @class ExperimentData

void ExperimentData::addData(nsx::sptrDataSet data)
{
    data_.append(data);
    if ((index_ < 0) || (index_ >= data_.size()))
        index_ = 0;
    gSession->onDataChanged();
}

void ExperimentData::removeSelectedData()
{
    if (data_.size() > 0)
        data_.removeAt(index_);
    if (index_ >= data_.size())
        index_--;
    gSession->onDataChanged();
}

void ExperimentData::selectData(int i)
{
    if (i >= 0 && i < data_.size())
        index_ = i;
    gSession->onDataChanged();
}

nsx::sptrDataSet ExperimentData::selectedData()
{
    if (data_.size() > 0 && index_ < data_.size() && index_ >= 0)
        return data_.at(index_);
    return nullptr;
}

nsx::DataList ExperimentData::allDataVector()
{
    nsx::DataList vector;
    for (nsx::sptrDataSet data : data_)
        vector.push_back(data);
    return vector;
}

void ExperimentData::convertToHDF5()
{
    // TODO: implement the function
}

//  ***********************************************************************************************
//! @class InstrumentModel

InstrumentModel::InstrumentModel(const QString& name, const QString& sourceName)
{
    // TODO: implement constructor
}

//  ***********************************************************************************************

UnitCellsModel::UnitCellsModel()
{}

nsx::sptrUnitCell UnitCellsModel::selectedCell()
{
    if (unitCells_.empty())
        return nullptr;
    if (selected_ >= unitCells_.size() || selected_ < 0)
        return unitCells_.at(0);
    return unitCells_.at(selected_);
}

//  ***********************************************************************************************
//! @class ExperimentModel

ExperimentModel::ExperimentModel(nsx::sptrExperiment experiment) : experiment_ {experiment}
{
    data_ = new ExperimentData;
    instrument_ = new InstrumentModel(QString::fromStdString(experiment_->name()), "sourceName");
    peaks_ = new PeaksModel;
    units_ = new UnitCellsModel;
}
