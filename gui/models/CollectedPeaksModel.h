#pragma once

#include <memory>
#include <string>
#include <vector>

#include <QAbstractTableModel>
#include <QModelIndexList>

#include <core/PeakList.h>
#include <core/DataTypes.h>
#include <core/Experiment.h>

class QObject;

class SessionModel;

class CollectedPeaksModel : public QAbstractTableModel
{
    Q_OBJECT
public:

    enum Column {h,k,l,px,py,frame,intensity,sigmaIntensity,numor,unitCell,count};

    explicit CollectedPeaksModel(SessionModel* session, nsx::sptrExperiment experiment,QObject* parent = 0);

    CollectedPeaksModel(SessionModel* session, nsx::sptrExperiment experiment, const nsx::PeakList& peaks, QObject *parent = 0);

    virtual int rowCount(const QModelIndex& parent) const override;

    virtual int columnCount(const QModelIndex& parent) const override;

    void reset();

    QVariant data(const QModelIndex &index, int role) const override;

    Qt::ItemFlags flags(const QModelIndex &index) const override;

    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    void setPeaks(const nsx::PeakList& peaks);

    const nsx::PeakList& peaks() const;

    bool indexIsValid(const QModelIndex& index) const;

    void sort(int column, Qt::SortOrder order) override;

    void normalizeToMonitor(double factor);

    QModelIndexList unindexedPeaks();

    QModelIndexList selectedPeaks();

    nsx::sptrExperiment experiment();

    void selectPeak(const QModelIndex& index);

    SessionModel* session();

    void togglePeakSelection(QModelIndex peak_index);

public slots:

    void slotChangeEnabledPeak(nsx::sptrPeak3D peak);

    void slotChangeMaskedPeaks(const nsx::PeakList& peaks);

    void slotRemoveUnitCell(const nsx::sptrUnitCell unit_cell);

    void setUnitCell(const nsx::sptrUnitCell& unitCell, QModelIndexList selectedPeaks=QModelIndexList());

    void sortEquivalents();

signals:

    void signalSelectedPeakChanged(nsx::sptrPeak3D peak);

    void unitCellUpdated();

private:

    void setSession(SessionModel* session);

private:

    SessionModel* _session;

    nsx::sptrExperiment _experiment;
    nsx::PeakList _peaks;
};
