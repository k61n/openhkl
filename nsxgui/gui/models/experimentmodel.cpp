
#include "nsxgui/gui/models/experimentmodel.h"
#include "nsxgui/gui/models/session.h"
#include "nsxgui/qcr/engine/logger.h"
#include "nsxgui/qcr/engine/mixin.h"
#include <core/Peak3D.h>
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

nsx::DataList ExperimentData::allDataVector()
{
    nsx::DataList vector;
    for (nsx::sptrDataSet data : data_)
        vector.push_back(data);
    return vector;
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
//! @class PeaksModel

PeaksModel::PeaksModel()
{}

nsx::PeakList PeaksModel::allPeaks()
{
    nsx::PeakList all;
    for (auto &peakList : peakLists_) {
        for (auto &&peak : peakList) {
            all.push_back(peak);
        }
    }
    return all;
}

void PeaksModel::appendPeaks(nsx::PeakList peaks)
{
    peakLists_.push_back(peaks);
}

void PeaksModel::autoAssignUnitCell()
{
    nsx::PeakList peaks = allPeaks(); //selectedPeaks();

    UnitCellsModel* unitCellModel = gSession->selectedExperiment()->unitCells();

    QList<nsx::sptrUnitCell> cells = unitCellModel->allUnitCells();

    if (cells.size() < 1) {
      gLogger->log("[INFO] There are no unit cells to assign");
      return;
    }

    for (nsx::sptrPeak3D peak : peaks) {
      if (!peak->enabled()) {
        continue;
      }

      Eigen::RowVector3d hkl;
      bool assigned = false;

      for (nsx::sptrUnitCell cell : cells) {
        nsx::MillerIndex hkl(peak->q(), *cell);
        if (hkl.indexed(cell->indexingTolerance())) {
          peak->setUnitCell(cell);
          assigned = true;
          break;
        }
      }

      // could not assign unit cell
      if (assigned == false) {
        peak->setSelected(false);
      }
    }
    gLogger->log("Done auto assigning unit cells");

    //emit model()->itemChanged(this);
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
    peaks_ = new PeaksModel;
}
