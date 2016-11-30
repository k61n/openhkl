#include "CollectedPeaksModel.h"
#include "CollectedPeaksProxyModel.h"

#include <iostream>

CollectedPeaksProxyModel::CollectedPeaksProxyModel(QObject *parent) :
    QSortFilterProxyModel(parent)
{

    CollectedPeaksModel* peaksModel = dynamic_cast<CollectedPeaksModel*>(sourceModel());
    if (peaksModel)
        _minIntensity = peaksModel->getMinIntensity();
}

bool CollectedPeaksProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const
{
    int column = sourceParent.column();
    const QModelIndex sourceIndex = mapToSource(sourceParent);

    std::cout<<sourceIndex.column()<<std::endl;

    switch (column)
    {
    case CollectedPeaksModel::Column::h:
    {
        QModelIndex index = sourceModel()->index(sourceRow, 1, sourceParent);
        if(sourceModel()->data(index).toDouble() < _minIntensity)
            return false;
        break;
    }
    case CollectedPeaksModel::Column::k:
    {
        QModelIndex index = sourceModel()->index(sourceRow, 1, sourceParent);
        if(sourceModel()->data(index).toDouble() < _minIntensity)
            return false;
        break;
    }
    case CollectedPeaksModel::Column::l:
    {
        QModelIndex index = sourceModel()->index(sourceRow, 1, sourceParent);
        if(sourceModel()->data(index).toDouble() < _minIntensity)
            return false;
        break;
    }
    case CollectedPeaksModel::Column::intensity:
    {
        QModelIndex index = sourceModel()->index(sourceRow, 1, sourceParent);
        if(sourceModel()->data(index).toDouble() < _minIntensity)
            return false;
        break;
    }
    case CollectedPeaksModel::Column::sigmaIntensity:
    {
        QModelIndex index = sourceModel()->index(sourceRow, 1, sourceParent);
        if(sourceModel()->data(index).toDouble() < _minIntensity)
            return false;
        break;
    }
    default:
    {
        return true;
    }
    }

    return true;
}

QVariant CollectedPeaksProxyModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    return sourceModel()->headerData(section, orientation,role);
}
