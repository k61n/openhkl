#include "include/Chemistry/DragElementModel.h"

DragElementModel::DragElementModel() : QAbstractTableModel(), _isotopes(), _abundances()
{
    _isotopesMgr = SX::Chemistry::IsotopeManager::Instance();
}

int DragElementModel::rowCount(const QModelIndex &parent) const
{
    if (_isotopes.empty())
        return 1;
    else
        return _isotopes.count();
}

int DragElementModel::columnCount(const QModelIndex &parent) const
{
    return 2;
}

QVariant DragElementModel::data(const QModelIndex &index, int role) const
{

    if (!index.isValid() || _isotopes.empty())
        return QVariant();

    if (role == Qt::DisplayRole)
    {
        if (index.column() == 0)
            return _isotopes.at(index.row());
        else
            return _abundances.at(index.row());
    }

    return QVariant();
}

bool DragElementModel::setData(const QModelIndex &index, const QVariant &value, int role)
{

    if (!index.isValid())
        return false;

    if (role==Qt::DisplayRole)
    {
        if (index.column() == 1)
            return false;

        SX::Chemistry::sptrIsotope isotope=_isotopesMgr->findIsotope(value.toString().toStdString());

        QString isName=QString::fromStdString(isotope->getName());
        double abundance=isotope->getAbundance();

        if (index.row() >= _isotopes.size())
        {
            _isotopes.append(isName);
            _abundances.append(abundance);
            std::cout<<"vdvds"<<std::endl;
            insertRow(index.row());
        }
        else
        {
            _isotopes.replace(index.row(),isName);
            _abundances.replace(index.row(),abundance);
        }

        emit dataChanged(index,index);

        return true;
    }

    return false;
}

Qt::ItemFlags DragElementModel::flags(const QModelIndex &index) const
{
    return QAbstractItemModel::flags(index) | Qt::ItemIsDropEnabled | Qt::ItemIsEditable;
}
