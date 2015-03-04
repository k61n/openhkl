#ifndef DRAGELEMENTMODEL_H
#define DRAGELEMENTMODEL_H

#include <QAbstractTableModel>
#include "IsotopeManager.h"

#include <QList>

class DragElementModel : public QAbstractTableModel
{
    Q_OBJECT

public:

    typedef QList<QPair<QString,double>> isotopesList;

    DragElementModel();

    bool insertRows(int row, int count, const QModelIndex &parent);

    int rowCount(const QModelIndex &parent=QModelIndex()) const;

    int columnCount(const QModelIndex &parent) const;

    QVariant headerData(int section, Qt::Orientation orientation, int role) const;

    QVariant data(const QModelIndex &index, int role) const;

    bool setData(const QModelIndex &index, const QVariant &value, int role=Qt::DisplayRole);

    Qt::ItemFlags flags(const QModelIndex &index) const;

    const isotopesList& getIsotopes() const;


private:
    SX::Chemistry::IsotopeManager* _isotopesMgr;
    isotopesList _isotopes;
};

#endif // DRAGELEMENTMODEL_H
