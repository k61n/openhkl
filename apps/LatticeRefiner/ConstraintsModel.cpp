#include <QDebug>

#include "ConstraintsModel.h"

ConstraintsModel::ConstraintsModel(QObject *parent) :
    QStandardItemModel(parent),
    _possibleConstraints()
{

}

void ConstraintsModel::setItem(int row, int column, QStandardItem *item)
{
    QStandardItemModel::setItem(row,column,item);
    _possibleConstraints.push_back(item->text());
}
