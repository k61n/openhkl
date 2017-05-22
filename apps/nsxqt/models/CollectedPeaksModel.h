#ifndef NSXQT_COLLECTEDPEAKSMODEL_H
#define NSXQT_COLLECTEDPEAKSMODEL_H

#include <memory>
#include <string>
#include <vector>

#include <QAbstractTableModel>
#include <QModelIndexList>

#include <nsxlib/crystal/CrystalTypes.h>
#include <nsxlib/data/DataTypes.h>

class QObject;

namespace nsx {
class Experiment;
}

class CollectedPeaksModel : public QAbstractTableModel
{
    Q_OBJECT
public:

    using sptrExperiment = std::shared_ptr<nsx::Experiment>;

    enum Column {h,k,l,intensity,sigmaIntensity,transmission,lorentzFactor,numor,selected,unitCell,count};

    explicit CollectedPeaksModel(sptrExperiment experiment,QObject* parent = 0);

    CollectedPeaksModel(sptrExperiment experiment, const nsx::PeakList& peaks, QObject *parent = 0);

    ~CollectedPeaksModel() = default;

    // todo(jonathan): virtual/override methods should not have default arguments!
    int rowCount(const QModelIndex &parent=QModelIndex()) const override;
    // todo(jonathan): virtual/override methods should not have default arguments!
    int columnCount(const QModelIndex &parent=QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;

    Qt::ItemFlags flags(const QModelIndex &index) const override;

    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    void addPeak(const nsx::sptrPeak3D& peak);

    void setPeaks(const nsx::DataList& data);
    void setPeaks(const nsx::PeakList& peaks);

    const nsx::PeakList& getPeaks() const;
    nsx::PeakList getPeaks(const QModelIndexList& indices) const;

    bool indexIsValid(const QModelIndex& index) const;

    void sort(int column, Qt::SortOrder order);

    void normalizeToMonitor(double factor);

    void writeShelX(const std::string& filename, QModelIndexList indices=QModelIndexList());

    void writeFullProf(const std::string& filename, QModelIndexList indices=QModelIndexList());

    void setUnitCells(const nsx::UnitCellList& cells);

    QModelIndexList getUnindexedPeaks();

    QModelIndexList getValidPeaks();

    sptrExperiment getExperiment();

public slots:
    void sortEquivalents();
    void setUnitCell(const nsx::sptrUnitCell& unitCell, QModelIndexList selectedPeaks=QModelIndexList());

signals:
    void unitCellUpdated();

private:
    sptrExperiment _experiment;
    nsx::PeakList _peaks;
    nsx::UnitCellList _cells;
};

#endif // NSXQT_COLLECTEDPEAKSMODEL_H
