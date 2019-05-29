
#ifndef PEAKLISTS_H
#define PEAKLISTS_H

#include <QList>
#include <build/core/include/core/CrystalTypes.h>

class FilteredPeaksModel {
public:
    FilteredPeaksModel(const QString& name, nsx::PeakList);

    QString getName() { return name_; }
    void setName(const QString& name) { name_ = name; }
    nsx::PeakList getPeaks() { return filteredPeaks_; }
private:
    QString name_;
    nsx::PeakList filteredPeaks_;
};

class PeakListsModel {
public:
    PeakListsModel(const QString& name, nsx::PeakList list);

    QString getName() { return name_; }
    void setName(const QString& name) { name_ = name; }
    nsx::PeakList getAllListPeaks() { return allPeaks_; }
    FilteredPeaksModel* getPeaksAt(int i);
    void addFilteredPeaks(const QString& name, nsx::PeakList peaks);
    int numberFilteredLists() { return filtered_.size(); }
private:
    QString name_;
    nsx::PeakList allPeaks_;
    QList<FilteredPeaksModel*> filtered_;
};

class PeaksModel {
public:
    PeaksModel();

    void integratePeaks();
    void buildShapeLibrary();
    void autoAssignUnitCell();
    PeakListsModel* selectedPeakLists(int i);
    nsx::PeakList allPeaks();
    QStringList peaklistNames();
    int numberLists() { return peakLists_.size(); }
    void addPeakListsModel(const QString& name, nsx::PeakList);
private:
    QList<PeakListsModel*> peakLists_;
};

#endif //PEAKLISTS_H
