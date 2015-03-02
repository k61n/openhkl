#ifndef DRAGELEMENTMODEL_H
#define DRAGELEMENTMODEL_H

#include <QAbstractTableModel>
#include "IsotopeManager.h"

#include <QList>

class DragElementModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    DragElementModel();

    int rowCount(const QModelIndex &parent) const;

    int columnCount(const QModelIndex &parent) const;

    QVariant data(const QModelIndex &index, int role) const;

    bool setData(const QModelIndex &index, const QVariant &value, int role=Qt::DisplayRole);

    Qt::ItemFlags flags(const QModelIndex &index) const;


private:
    SX::Chemistry::IsotopeManager* _isotopesMgr;
    QList<QString> _isotopes;
    QList<double> _abundances;

};

#endif // DRAGELEMENTMODEL_H
