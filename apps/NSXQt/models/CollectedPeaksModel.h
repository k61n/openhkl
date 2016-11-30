#ifndef COLLECTEDPEAKSMODEL_H
#define COLLECTEDPEAKSMODEL_H

#include <memory>
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

    int rowCount(const QModelIndex &parent) const override;

    int columnCount(const QModelIndex &parent) const override;

    QVariant data(const QModelIndex &index, int role) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;

    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    void addPeak(sptrPeak3D peak);

    void setPeaks(const std::vector<sptrPeak3D>& peaks);

    bool indexIsValid(const QModelIndex& index) const;

    double getMinIntensity() const;

    double getMinSigmaIntensity() const;

signals:

public slots:

private:

    std::vector<sptrPeak3D> _peaks;

};

#endif // COLLECTEDPEAKSMODEL_H
