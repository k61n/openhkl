#ifndef COLLECTEDPEAKSMODEL_H
#define COLLECTEDPEAKSMODEL_H

#include <memory>
#include <string>
#include <vector>

#include <QAbstractTableModel>

#include "Peak3D.h"

using namespace SX::Crystal;

class CollectedPeaksModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    enum Column {h,k,l,intensity,sigmaIntensity,transmission,lorentzFactor,numor,selected,observed,count};

    explicit CollectedPeaksModel(QObject* parent = 0);

    CollectedPeaksModel(const std::vector<sptrPeak3D>& peaks, QObject *parent = 0);

    ~CollectedPeaksModel();

    int rowCount(const QModelIndex &parent=QModelIndex()) const override;

    int columnCount(const QModelIndex &parent=QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;

    void setData(const std::vector<std::shared_ptr<SX::Data::IData>>& data);

    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    void addPeak(sptrPeak3D peak);

    void setPeaks(const std::vector<sptrPeak3D>& peaks);

    const std::vector<sptrPeak3D>& getPeaks() const;

    bool indexIsValid(const QModelIndex& index) const;

    void sort(int column, Qt::SortOrder order);

    void normalizeToMonitor(double factor);

    void writeShelX(const std::string& filename, double tolerance=0.2, QModelIndexList indexes=QModelIndexList());

    void writeFullProf(const std::string& filename, double tolerance=0.2, QModelIndexList indexes=QModelIndexList());

public slots:

    void sortEquivalents();


private:

    std::vector<sptrPeak3D> _peaks;

};

#endif // COLLECTEDPEAKSMODEL_H
