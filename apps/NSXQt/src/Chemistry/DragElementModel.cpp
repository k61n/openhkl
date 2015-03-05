#include "include/Chemistry/DragElementModel.h"
#include "include/Chemistry/IsotopesListWidget.h"
#include "include/Chemistry/ElementsListWidget.h"

#include "Element.h"
#include "ElementManager.h"
#include "Isotope.h"
#include "IsotopeManager.h"

DragElementModel::DragElementModel() : QAbstractTableModel(), _isotopes(), _sender(nullptr)
{
}

int DragElementModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    if (_isotopes.empty())
        return 1;
    else
        return _isotopes.size()+1;
}

int DragElementModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return 2;
}

QVariant DragElementModel::data(const QModelIndex &index, int role) const
{

    if (!index.isValid() || _isotopes.empty())
        return QVariant();

    if (role == Qt::DisplayRole)
    {
        if (index.row()>=_isotopes.size())
            return QVariant();

        QPair<QString,double> pair=_isotopes.at(index.row());

        if (index.column() == 0)
            return pair.first;
        else
            return pair.second;
    }

    return QVariant();
}

bool DragElementModel::insertRows(int row, int count, const QModelIndex &parent)
{
    Q_UNUSED(parent);

    beginInsertRows(QModelIndex(),row,row+count-1);
    endInsertRows();
    return true;

}

bool DragElementModel::removeRows(int position, int rows, const QModelIndex &index)
{
    Q_UNUSED(index);
    beginRemoveRows(QModelIndex(), position, position+rows-1);
    endRemoveRows();
    return true;
}
bool DragElementModel::setData(const QModelIndex &index, const QVariant &value, int role)
{

    if (!index.isValid())
        return false;

    if (role==Qt::DisplayRole)
    {

        if (!_sender)
            return false;

        if (dynamic_cast<IsotopesListWidget*>(_sender))
        {

            SX::Chemistry::IsotopeManager* imgr = SX::Chemistry::IsotopeManager::Instance();

            SX::Chemistry::sptrIsotope isotope=imgr->getIsotope(value.toString().toStdString());
            QString name=QString::fromStdString(isotope->getName());
            for (auto it : _isotopes)
            {
                // If the dropped isotope is already in the list, do nothing
                if (it.first.compare(name)==0)
                    return false;
            }
            double abundance=isotope->getAbundance();
            QPair<QString,double> pair(name,abundance);

            if (index.row() >= _isotopes.size())
            {
                _isotopes.append(pair);
                insertRow(rowCount());
            }
            else
                _isotopes.replace(index.row(),pair);
        }
        else if (dynamic_cast<ElementsListWidget*>(_sender))
        {
            removeRows(1,rowCount());
            _isotopes.clear();
            SX::Chemistry::ElementManager* emgr = SX::Chemistry::ElementManager::Instance();
            SX::Chemistry::sptrElement element=emgr->getElement(value.toString().toStdString());
            for (auto p : element->getIsotopes())
            {
                QPair<QString,double> pair(QString::fromStdString(p.first),p.second->getAbundance());
                _isotopes.append(pair);
                insertRow(rowCount());
            }
        }
        else
            return false;

        emit dataChanged(index,index);

        return true;
    }
    else if (role==Qt::EditRole)
    {
        if (index.column()==0)
        {
            QString name=value.toString();
            for (auto it : _isotopes)
            {
                // If the dropped isotope is already in the list, do nothing
                if (it.first.compare(name)==0)
                    return false;
            }
        }

        if (index.row()>=_isotopes.size())
        {
            if (index.column()==1)
                return false;
            QPair<QString,double> pair(value.toString(),0.0);
            _isotopes.append(pair);
            insertRow(rowCount());
        }
        else
        {
            if (index.column()==0)
                _isotopes[index.row()].first=value.toString();
            else
            {
                double abundance=value.toDouble();
                // If the abundance if not in [0,1], do nothing
                if (abundance<0||abundance>1.0)
                    return false;
                _isotopes[index.row()].second = abundance;
            }
        }
        emit dataChanged(index,index);
        return true;
    }

    return false;
}

QVariant DragElementModel::headerData(int section, Qt::Orientation orientation, int role) const
 {
     if (role != Qt::DisplayRole)
         return QVariant();

     if (orientation == Qt::Horizontal) {
         switch (section) {
             case 0:
                 return tr("Isotope");

             case 1:
                 return tr("Abundance (in [0,1])");

             default:
                 return QVariant();
         }
     }
     return QVariant();
}

Qt::ItemFlags DragElementModel::flags(const QModelIndex &index) const
{
    // The first column contains the isotope name: can not be edited
    if (index.column()==0)
        return QAbstractItemModel::flags(index) | Qt::ItemIsEditable | Qt::ItemIsDropEnabled;
    // The second column contains the isotope abundance can not be dropped
    else
        return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;

}

const DragElementModel::isotopesList& DragElementModel::getIsotopes() const
{
    return _isotopes;
}

void DragElementModel::setSender(QObject* sender)
{
    _sender = sender;
}
