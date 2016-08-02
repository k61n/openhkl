#include "DragElementModel.h"
#include "IsotopesListWidget.h"
#include "ElementsListWidget.h"

#include "Element.h"
#include "ElementManager.h"
#include "Isotope.h"
#include "IsotopeManager.h"

DragElementModel::DragElementModel()
: QAbstractTableModel(),
  _isotopes(),
  _elementMgr(SX::Chemistry::ElementManager::Instance()),
  _sender(nullptr)
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

    if (!index.isValid())
        return QVariant();

    if (role==Qt::DisplayRole)
    {

        if (index.row()<0)
            return QVariant();

        if (_isotopes.empty())
        {
            if (index.column() == 0)
                return "drag elements/isotopes here";
            else
                return 0.0;
        }

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

    if (count != 1)
        return false;

    beginInsertRows(QModelIndex(),row,row);
    _isotopes.insert(row,QPair<QString,double>());
    endInsertRows();
    return true;
}

bool DragElementModel::removeRows(int row, int count, const QModelIndex &index)
{
    Q_UNUSED(index);

    if (_isotopes.empty())
        return false;

    if (row==_isotopes.size())
        return false;

    beginRemoveRows(QModelIndex(), row, row+count-1);
    for (int idx=0;idx<count;++idx)
        _isotopes.removeAt(row);
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
            double abundance=isotope->getNaturalAbundance();
            QPair<QString,double> pair(name,abundance);

            if (index.row() == _isotopes.size())
                insertRow(rowCount());
            _isotopes.replace(index.row(),pair);
        }
        else if (dynamic_cast<ElementsListWidget*>(_sender))
        {

            removeRows(0,_isotopes.size());

            SX::Chemistry::sptrElement element=_elementMgr->getElement(value.toString().toStdString());

            const SX::Chemistry::isotopeMap& isotopes=element->getIsotopes();
            const SX::Chemistry::contentsMap& abundances=element->getAbundances();
            auto ait=abundances.begin();
            unsigned int comp(0);
            for (auto it=isotopes.begin();it!=isotopes.end();++it,++ait)
            {
                QPair<QString,double> pair(QString::fromStdString(it->first),ait->second);
                insertRow(comp);
                _isotopes.replace(comp++,pair);
            }
        }
        else
            return false;

        emit dataChanged(index,index);

        return true;
    }
    else if (role==Qt::EditRole)
    {
        double abundance=value.toDouble();
        // If the abundance if not in [0,1], do nothing
        if (abundance<0||abundance>1.0)
            return false;
        _isotopes[index.row()].second = abundance;
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
        return QAbstractItemModel::flags(index) | Qt::ItemIsDropEnabled;
    // The second column contains the isotope abundance can not be dropped
    else
        return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;

}

void DragElementModel::setSender(QObject* sender)
{
    _sender = sender;
}

void DragElementModel::buildElement(const QString& elementName)
{
    // Build the element from the registry
    SX::Chemistry::sptrElement element=_elementMgr->getElement(elementName.toStdString());

    for (const auto& p : _isotopes)
        element->addIsotope(p.first.toStdString(),p.second);
}

