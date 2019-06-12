//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/models/PeakLists.cpp
//! @brief     Implements classes FilteredPeaksModel, PeakListsModel, PeaksModel
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "gui/models/PeakLists.h"
#include "gui/models/Session.h"
#include <QCR/engine/logger.h>
#include "core/experiment/DataSet.h"
#include "core/peak/Peak3D.h"
#include "core/raw/IDataReader.h"
#include <QInputDialog>

FilteredPeaksModel::FilteredPeaksModel(const QString& name, nsx::PeakList list)
    : name_ {name}, filteredPeaks_ {list}
{
}

void FilteredPeaksModel::removePeaks(nsx::sptrDataSet removedData)
{
    nsx::PeakList newList;
    for (nsx::sptrPeak3D peak : filteredPeaks_) {
        if (peak->data() != removedData) {
            newList.push_back(peak);
        }
    }
    filteredPeaks_ = newList;
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
    selected = 0;
    gSession->onPeaksChanged();
}

void PeakListsModel::removeFilteredPeaks(int i)
{
    if (i < 0 || i >= filtered_.size()) {
        gLogger->log("[ERROR] could not remove a filtered peak list");
        return;
    }
    gLogger->log("[INFO] Removing filtered peak list " + filtered_.at(i)->getName());
    filtered_.removeAt(i);
    if (filtered_.empty()) {
        selected = -1;
    } else {
        selected = 0;
    }
    gSession->onPeaksChanged();
}

void PeakListsModel::removePeaks(nsx::sptrDataSet removedData)
{
    for (FilteredPeaksModel* fpm : filtered_)
        fpm->removePeaks(removedData);
    remakePeakLists();
}

void PeakListsModel::selectList(int i)
{
    if (i<0 || i>=filtered_.size()) {
        selected = 0;
        return;
    }
    selected = i;
}

FilteredPeaksModel* PeakListsModel::selectedFilteredList()
{
    if (selected < 0 || filtered_.empty())
        return nullptr;
    return filtered_.at(selected);
}

void PeakListsModel::remakePeakLists()
{
    for (int i = filtered_.size()-1; i>=0; i--) {
        if (filtered_.at(i)->getPeaks().empty())
            removeFilteredPeaks(i);
    }
}

//  ***********************************************************************************************
//! @class PeaksModel

PeaksModel::PeaksModel() {}

nsx::PeakList PeaksModel::allPeaks()
{
    nsx::PeakList all;
    for (PeakListsModel* peakList : peakLists_) {
        nsx::PeakList peaks = peakList->getAllListPeaks();
        all.insert(all.end(), peaks.begin(), peaks.end());
    }
    return all;
}

void PeaksModel::selectPeakLists(int i)
{
    if (peakLists_.empty())
        return;
    if (i<0 || i>= peakLists_.size())
        return;
    selectedLists = i;
}

PeakListsModel* PeaksModel::selectedPeakLists(int i)
{
    if (peakLists_.empty()) {
        gLogger->log("[ERROR] No peaklist selected");
        return nullptr;
    }
    if (i < 0 || i > peakLists_.size())
        return peakLists_.at(selectedLists);
    return peakLists_.at(i);
}

void PeaksModel::addPeakListsModel(const QString& name, nsx::PeakList list)
{
    peakLists_.append(new PeakListsModel(name, list));
    selectedLists = peakLists_.size()-1;
}

QStringList PeaksModel::peaklistNames()
{
    QStringList foundListsNames;
    for (PeakListsModel* peaklist : peakLists_)
        foundListsNames.append(peaklist->getName());
    return foundListsNames;
}

QStringList PeaksModel::allFilteredListNames()
{
    QStringList filteredListNames;
    for (PeakListsModel* model : peakLists_) {
        QString prefix = model->getName();
        for (int i = 0; i < model->numberFilteredLists(); i++)
            filteredListNames.append(prefix + "/" + model->getPeaksAt(i)->getName());
    }
    return filteredListNames;
}

void PeaksModel::removePeakListsModel(int i)
{

    if (i < 0 || i >= peakLists_.size()) {
        gLogger->log("[ERROR]  could not remove peaklistsmodel");
        return;
    }

    if (peakLists_.empty()) {
        gLogger->log("[ERROR] No Peaklists to remove");
        return;
    }
    if (i<0 || i>=peakLists_.size()) {
        i = selectedLists;
    }
    gLogger->log("[INFO] removing peaklistsmodel " + peakLists_.at(i)->getName());
    peakLists_.removeAt(i);
    if (peakLists_.empty()) {
        selectedLists = -1;
    } else {
        selectedLists = 0;
    }
    gSession->onPeaksChanged();
}

void PeaksModel::removePeaks(nsx::sptrDataSet removedData)
{
    for (PeakListsModel* plm : peakLists_)
        plm->removePeaks(removedData);
    remakePeakLists();
}

void PeaksModel::normalizeToMonitor()
{
    if (peakLists_.empty())
        return;
    bool ok;
    double factor = QInputDialog::getDouble(
                nullptr, "Enter normalization factor", "", 1.0e4, 1.0e-9, 1.0e9, 3, &ok);
    if (!ok)
        return;
    nsx::PeakList selectedPeaks = peakLists_.at(selectedLists)->selectedFilteredList()->getPeaks();
    for (nsx::sptrPeak3D peak : selectedPeaks) {
        nsx::sptrDataSet data = peak->data();
        if (!data)
            continue;
        double monitor = data->reader()->metadata().key<double>("monitor");
        peak->setScale(factor/monitor);
    }
    gSession->onPeaksChanged();
}

void PeaksModel::remakePeakLists()
{
    for (int i = peakLists_.size()-1; i>=0; i--) {
        if (peakLists_.at(i)->numberFilteredLists() == 0) {
            removePeakListsModel(i);
        }
    }
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
