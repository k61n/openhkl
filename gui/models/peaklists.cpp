//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/models/peaklists.cpp
//! @brief     Implements ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "gui/models/peaklists.h"
#include "gui/models/session.h"
#include <QCR/engine/logger.h>

FilteredPeaksModel::FilteredPeaksModel(const QString& name, nsx::PeakList list)
    : name_ {name}, filteredPeaks_ {list}
{
}

//  ***********************************************************************************************
//! @class PeakListsModel

PeakListsModel::PeakListsModel(const QString& name, nsx::PeakList list)
    : name_ {name}, allPeaks_ {list}
{
    addFilteredPeaks("all peaks", list);
}

FilteredPeaksModel* PeakListsModel::getPeaksAt(int i)
{
    if (i < 0 || i > filtered_.size())
        return nullptr;
    return filtered_.at(i);
}

void PeakListsModel::addFilteredPeaks(const QString& name, nsx::PeakList peaks)
{
    filtered_.append(new FilteredPeaksModel(name, peaks));
    gSession->onPeaksChanged();
}

void PeakListsModel::removeFilteredPeaks(int i)
{
    if (i<0 || i>=filtered_.size()) {
        gLogger->log("[ERROR] could not remove a filtered peak list");
        return;
    }
    gLogger->log("[INFO] Removing filtered peak list " + filtered_.at(i)->getName());
    filtered_.removeAt(i);
    gSession->onPeaksChanged();
}

//  ***********************************************************************************************
//! @class PeaksModel

PeaksModel::PeaksModel() {}

nsx::PeakList PeaksModel::allPeaks()
{
    nsx::PeakList all;
    for (auto& peakList : peakLists_) {
        nsx::PeakList peaks = peakList->getAllListPeaks();
        all.insert(all.end(), peaks.begin(), peaks.end());
    }
    return all;
}

PeakListsModel* PeaksModel::selectedPeakLists(int i)
{
    if (i < 0 || i > peakLists_.size())
        return nullptr;
    return peakLists_.at(i);
}

void PeaksModel::addPeakListsModel(const QString& name, nsx::PeakList list)
{
    peakLists_.append(new PeakListsModel(name, list));
}

QStringList PeaksModel::peaklistNames()
{
    QStringList foundListsNames;
    for (auto& peaklist : peakLists_) {
        foundListsNames.append(peaklist->getName());
    }
    return foundListsNames;
}

QStringList PeaksModel::allFilteredListNames()
{
    QStringList filteredListNames;
    for (PeakListsModel* model : peakLists_) {
        QString prefix = model->getName();
        for (int i=0; i<model->numberFilteredLists(); i++)
            filteredListNames.append(prefix+"/" +model->getPeaksAt(i)->getName());
    }
    return filteredListNames;
}

void PeaksModel::removePeakListsModel(int i)
{
    if (i<0 || i>=peakLists_.size()) {
        gLogger->log("[ERROR]  could not remove peaklistsmodel");
        return;
    }
    gLogger->log("[INFO] removing peaklistsmodel " + peakLists_.at(i)->getName());
    peakLists_.removeAt(i);
    gSession->onPeaksChanged();
}


// void PeaksModel::autoAssignUnitCell()
//{
// maybe move to gSession

//    nsx::PeakList peaks = allPeaks(); //selectedPeaks();

//    UnitCellsModel* unitCellModel = gSession->selectedExperiment()->unitCells();

//    QList<nsx::sptrUnitCell> cells = unitCellModel->allUnitCells();

//    if (cells.size() < 1) {
//      gLogger->log("[INFO] There are no unit cells to assign");
//      return;
//    }

//    for (nsx::sptrPeak3D peak : peaks) {
//      if (!peak->enabled()) {
//        continue;
//      }

//      Eigen::RowVector3d hkl;
//      bool assigned = false;

//      for (nsx::sptrUnitCell cell : cells) {
//        nsx::MillerIndex hkl(peak->q(), *cell);
//        if (hkl.indexed(cell->indexingTolerance())) {
//          peak->setUnitCell(cell);
//          assigned = true;
//          break;
//        }
//      }

//      // could not assign unit cell
//      if (assigned == false) {
//        peak->setSelected(false);
//      }
//    }
//    gLogger->log("Done auto assigning unit cells");

// emit model()->itemChanged(this);
//}
