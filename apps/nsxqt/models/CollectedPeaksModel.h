#pragma once

#include <memory>
#include <string>
#include <vector>

#include <QAbstractTableModel>
#include <QModelIndexList>

#include <nsxlib/CrystalTypes.h>
#include <nsxlib/DataTypes.h>
#include <nsxlib/InstrumentTypes.h>

class QObject;

class CollectedPeaksModel : public QAbstractTableModel
{
    Q_OBJECT
public:

    enum Column {h,k,l,intensity,sigmaIntensity,i_over_sigmai,transmission,lorentzFactor,numor,selected,unitCell,count};

    explicit CollectedPeaksModel(nsx::sptrExperiment experiment,QObject* parent = 0);

    CollectedPeaksModel(nsx::sptrExperiment experiment, const nsx::PeakList& peaks, QObject *parent = 0);

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

    void setPeaks(const nsx::PeakList& peaks);

    const nsx::PeakList& peaks() const;
    nsx::PeakList peaks(const QModelIndexList& indices) const;

    bool indexIsValid(const QModelIndex& index) const;

    void sort(int column, Qt::SortOrder order) override;

    void normalizeToMonitor(double factor);

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
    nsx::PeakList _peaks;
    nsx::UnitCellList _cells;
};
