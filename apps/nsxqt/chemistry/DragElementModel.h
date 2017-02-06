#ifndef DRAGELEMENTMODEL_H
#define DRAGELEMENTMODEL_H

#include <QAbstractTableModel>

#include <QList>

#include <nsxlib/chemistry/ElementManager.h>

class DragElementModel : public QAbstractTableModel
{
    Q_OBJECT

public:

    typedef QList<QPair<QString,double>> isotopesList;

    DragElementModel();

    bool insertRows(int row, int count, const QModelIndex &index=QModelIndex());
    bool removeRows(int position, int rows, const QModelIndex &index=QModelIndex());

    int rowCount(const QModelIndex &parent=QModelIndex()) const;

    int columnCount(const QModelIndex &parent) const;

    QVariant headerData(int section, Qt::Orientation orientation, int role) const;

    QVariant data(const QModelIndex &index, int role) const;

    bool setData(const QModelIndex &index, const QVariant &value, int role=Qt::DisplayRole);

    Qt::ItemFlags flags(const QModelIndex &index) const;

    void setSender(QObject* sender);

    //! Build the element whom isotopes contents is currently stored by the model
    void buildElement(const QString& elementName);


private:
    isotopesList _isotopes;
    SX::Chemistry::ElementManager* _elementMgr;
    QObject* _sender;
};

#endif // DRAGELEMENTMODEL_H
