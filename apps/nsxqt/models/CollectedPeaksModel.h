#ifndef NSXQT_COLLECTEDPEAKSMODEL_H
#define NSXQT_COLLECTEDPEAKSMODEL_H

#include <memory>
#include <string>
#include <vector>

#include <QAbstractTableModel>
#include <QModelIndexList>

#include <nsxlib/crystal/Peak3D.h>
#include <nsxlib/instrument/Sample.h>
#include <nsxlib/utils/Types.h>

class QObject;

class CollectedPeaksModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    enum Column {h,k,l,intensity,sigmaIntensity,transmission,lorentzFactor,numor,selected,unitCell,count};

    explicit CollectedPeaksModel(nsx::sptrExperiment experiment,QObject* parent = 0);

    CollectedPeaksModel(nsx::sptrExperiment experiment, const std::vector<nsx::sptrPeak3D>& peaks, QObject *parent = 0);

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

    void setPeaks(const std::vector<std::shared_ptr<nsx::DataSet>>& data);
    void setPeaks(const std::vector<nsx::sptrPeak3D>& peaks);

    const std::vector<nsx::sptrPeak3D>& getPeaks() const;
    std::vector<nsx::sptrPeak3D> getPeaks(const QModelIndexList& indices) const;

    bool indexIsValid(const QModelIndex& index) const;

    void sort(int column, Qt::SortOrder order);

    void normalizeToMonitor(double factor);

    void writeShelX(const std::string& filename, QModelIndexList indices=QModelIndexList());

    void writeFullProf(const std::string& filename, QModelIndexList indices=QModelIndexList());

    void setUnitCells(const nsx::CellList& cells);

    QModelIndexList getUnindexedPeaks();

    QModelIndexList getValidPeaks();

    nsx::sptrExperiment getExperiment();

public slots:
    void sortEquivalents();
    void setUnitCell(const nsx::sptrUnitCell& unitCell, QModelIndexList selectedPeaks=QModelIndexList());

signals:
    void unitCellUpdated();

private:
    nsx::sptrExperiment _experiment;
    std::vector<nsx::sptrPeak3D> _peaks;
    nsx::CellList _cells;
};

#endif // NSXQT_COLLECTEDPEAKSMODEL_H
