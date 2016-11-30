#ifndef COLLECTEDPEAKSPROXYMODEL_H
#define COLLECTEDPEAKSPROXYMODEL_H

#include <QSortFilterProxyModel>

class CollectedPeaksProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    explicit CollectedPeaksProxyModel(QObject *parent = 0);

    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override;

    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

signals:

public slots:

private:

    int _minh, _mink, _minl, _minIntensity, _minSigmaIntensity, _minTransmission, _minLorentFactor;

};

#endif // COLLECTEDPEAKSPROXYMODEL_H
