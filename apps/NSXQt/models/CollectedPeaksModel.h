#ifndef COLLECTEDPEAKSMODEL_H
#define COLLECTEDPEAKSMODEL_H

#include <memory>
#include <string>
#include <vector>

#include <QAbstractTableModel>

#include "Sample.h"
#include "Peak3D.h"
#include "Types.h"

using namespace SX::Crystal;
using namespace SX::Instrument;

class CollectedPeaksModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    enum Column {h,k,l,intensity,sigmaIntensity,transmission,lorentzFactor,numor,selected,unitCell,count};

    explicit CollectedPeaksModel(sptrExperiment experiment,QObject* parent = 0);

    CollectedPeaksModel(sptrExperiment experiment, const std::vector<sptrPeak3D>& peaks, QObject *parent = 0);

    ~CollectedPeaksModel();

    int rowCount(const QModelIndex &parent=QModelIndex()) const override;

    int columnCount(const QModelIndex &parent=QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;

    Qt::ItemFlags flags(const QModelIndex &index) const override;

    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    void addPeak(sptrPeak3D peak);

    void setPeaks(const std::vector<std::shared_ptr<SX::Data::IData>>& data);
    void setPeaks(const std::vector<sptrPeak3D>& peaks);

    const std::vector<sptrPeak3D>& getPeaks() const;
    std::vector<sptrPeak3D> getPeaks(const QModelIndexList& indexes) const;

    bool indexIsValid(const QModelIndex& index) const;

    void sort(int column, Qt::SortOrder order);

    void normalizeToMonitor(double factor);

    void writeShelX(const std::string& filename, QModelIndexList indexes=QModelIndexList());

    void writeFullProf(const std::string& filename, QModelIndexList indexes=QModelIndexList());

    void setUnitCells(const CellList& cells);

    QModelIndexList getUnindexedPeaks();

    QModelIndexList getValidPeaks();

    sptrExperiment getExperiment();

public slots:

    void sortEquivalents();
    void setUnitCell(sptrUnitCell unitCell, QModelIndexList selectedPeaks=QModelIndexList());

signals:

    void unitCellUpdated();

private:

    sptrExperiment _experiment;

    std::vector<sptrPeak3D> _peaks;

    CellList _cells;
};

#endif // COLLECTEDPEAKSMODEL_H
