//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/models/PeakLists.h
//! @brief     Defines classes FilteredPeaksModel, PeakListsModel, PeaksModel
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef GUI_MODELS_PEAKLISTS_H
#define GUI_MODELS_PEAKLISTS_H

#include "core/peak/Peak3D.h"
#include <QList>

//! Controls a PeakList
class FilteredPeaksModel {
 public:
    FilteredPeaksModel(const QString& name, nsx::PeakList);

    QString getName() { return name_; }
    void setName(const QString& name) { name_ = name; }
    nsx::PeakList getPeaks() { return filteredPeaks_; }
    void removePeaks(nsx::sptrDataSet removedData);

 private:
    QString name_;
    nsx::PeakList filteredPeaks_;
};

//! Controls the the `FilteredPeaksModel`s
class PeakListsModel {
 public:
    PeakListsModel(const QString& name, nsx::PeakList list);

    QString getName() { return name_; }
    void setName(const QString& name) { name_ = name; }
    nsx::PeakList getAllListPeaks() { return allPeaks_; }
    FilteredPeaksModel* getPeaksAt(int i);
    void addFilteredPeaks(const QString& name, nsx::PeakList peaks);
    int numberFilteredLists() { return filtered_.size(); }
    void removeFilteredPeaks(int i);
    void selectList(int i);
    FilteredPeaksModel* selectedFilteredList();
    void removePeaks(nsx::sptrDataSet removedData);

 private:
    void remakePeakLists();
    QString name_;
    nsx::PeakList allPeaks_;
    QList<FilteredPeaksModel*> filtered_;
    int selected;
};

//! Controls the `PeakListsModel`s
class PeaksModel {
 public:
    PeaksModel();

    void normalizeToMonitor();
    void integratePeaks();
    void buildShapeLibrary();
    void autoAssignUnitCell();
    void selectPeakLists(int i);
    PeakListsModel* selectedPeakLists(int i = -1);
    nsx::PeakList allPeaks();
    QStringList peaklistNames();
    QStringList allFilteredListNames();
    int numberLists() { return peakLists_.size(); }
    void addPeakListsModel(const QString& name, nsx::PeakList);
    void removePeakListsModel(int i = -1);
    void removePeaks(nsx::sptrDataSet removedData);

 private:
    void remakePeakLists();
    QList<PeakListsModel*> peakLists_;
    int selectedLists;
};

#endif // GUI_MODELS_PEAKLISTS_H
