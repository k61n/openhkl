#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>

#include <QIcon>
#include <QString>

#include "CollectedPeaksModel.h"
#include "IData.h"
#include "Sample.h"
#include "Types.h"

#include <QtDebug>

CollectedPeaksModel::CollectedPeaksModel(QObject *parent)
: QAbstractTableModel(parent)
{
}

CollectedPeaksModel::CollectedPeaksModel(const std::vector<sptrPeak3D> &peaks, QObject *parent)
: _peaks(peaks),
  QAbstractTableModel(parent)
{
}

CollectedPeaksModel::~CollectedPeaksModel()
{
}

void CollectedPeaksModel::addPeak(sptrPeak3D peak)
{
    auto it=std::find(_peaks.begin(),_peaks.end(),peak);
    if (it!=_peaks.end())
        return;

    _peaks.push_back(peak);
}

void CollectedPeaksModel::setPeaks(const std::vector<sptrPeak3D> & peaks)
{
    _peaks = peaks;
}

void CollectedPeaksModel::setPeaks(const std::vector<std::shared_ptr<SX::Data::IData> > &data)
{
    _peaks.clear();

    for (auto ptr : data)
    {
        // Add peaks present in this numor to the model
        for (sptrPeak3D peak : ptr->getPeaks())
            _peaks.push_back(peak);
    }
}

const std::vector<sptrPeak3D>& CollectedPeaksModel::getPeaks() const
{
    return _peaks;
}

int CollectedPeaksModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return _peaks.size();
}

int CollectedPeaksModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return Column::count;
}

Qt::ItemFlags CollectedPeaksModel::flags(const QModelIndex &index) const
{
    if (!indexIsValid(index))
        return Qt::ItemIsEnabled;

    int col = index.column();

    if (col == CollectedPeaksModel::Column::selected || col == CollectedPeaksModel::Column::observed)
        return QAbstractTableModel::flags(index) | Qt::ItemIsEnabled | Qt::ItemIsUserCheckable;

    else if (col == CollectedPeaksModel::Column::unitCell)
        return QAbstractTableModel::flags(index) | Qt::ItemIsEditable;

    else
        return QAbstractTableModel::flags(index);
}

QVariant CollectedPeaksModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal)
        switch (section)
        {
        case Column::h:
            return QString("h");
        case Column::k:
            return QString("k");
        case Column::l:
            return QString("l");
        case Column::intensity:
            return QString("intensity");
        case Column::sigmaIntensity:
            return QString((QChar)0x03C3)+"(intensity)";
        case Column::transmission:
            return QString("transmission");
        case Column::lorentzFactor:
            return QString("lorentz factor");
        case Column::numor:
            return QString("numor");
        case Column::selected:
            return QString("selected");
        case Column::observed:
            return QString("observed");
        case Column::unitCell:
            return QString("unit cell");
        default:
            return QVariant();
        }
    else
        return QString("Peak %1").arg(section+1);
}

QVariant CollectedPeaksModel::data(const QModelIndex &index, int role) const
{
    if (!indexIsValid(index))
        return QVariant();

    int row = index.row();
    int column = index.column();
    if (role==Qt::DisplayRole)
    {
        switch (column)
        {
        case Column::h:
        {
            const Eigen::RowVector3d& hkl = _peaks[row]->getMillerIndices();
            return hkl(0);
        }
        case Column::k:
        {
            const Eigen::RowVector3d& hkl = _peaks[row]->getMillerIndices();
            return hkl(1);
        }
        case Column::l:
        {
            const Eigen::RowVector3d& hkl = _peaks[row]->getMillerIndices();
            return hkl(2);
        }
        case Column::intensity:
        {
            double lorentzFactor = _peaks[row]->getLorentzFactor();
            double transmissionFactor = _peaks[row]->getTransmission();
            double scaledIntensity=_peaks[row]->getScaledIntensity()/lorentzFactor/transmissionFactor;
            return scaledIntensity;
        }
        case Column::sigmaIntensity:
        {
            double lorentzFactor = _peaks[row]->getLorentzFactor();
            double transmissionFactor = _peaks[row]->getTransmission();
            double sigmaScaledIntensity=_peaks[row]->getScaledSigma()/lorentzFactor/transmissionFactor;
            return sigmaScaledIntensity;
        }
        case Column::transmission:
        {
            double transmissionFactor = _peaks[row]->getTransmission();
            return transmissionFactor;
        }
        case Column::lorentzFactor:
        {
            return _peaks[row]->getLorentzFactor();
        }
        case Column::numor:
        {
            return _peaks[row]->getData()->getMetadata()->getKey<int>("Numor");
        }
        case Column::unitCell:
        {
            auto unitCell = _peaks[row]->getActiveUnitCell();
            if (!unitCell)
                return QString("not set");
            else
                return QString::fromStdString(unitCell->getName());
        }
        }
    }
    else if (role == Qt::CheckStateRole)
    {
        switch (column)
        {
        case Column::selected:
        {
            return _peaks[row]->isSelected();
        }
        case Column::observed:
        {
            return _peaks[row]->isObserved();
        }
        }
    }
    else if (role == Qt::UserRole)
    {
        if (column == Column::unitCell)
        {
            QStringList cellNames;
            for (auto cell : _cells)
                cellNames.append(QString::fromStdString(cell->getName()));
            return cellNames;
        }
    }
    return QVariant::Invalid;
}

void CollectedPeaksModel::sort(int column, Qt::SortOrder order)
{
    switch(column)
    {
    case Column::h:
    {
        std::sort(_peaks.begin(),_peaks.end(),
              [&](sptrPeak3D p1, sptrPeak3D p2)
              {
                auto hkl1 = p1->getMillerIndices();
                auto hkl2 = p2->getMillerIndices();
                return (hkl1[0]<hkl2[0]);
              }
              );
        break;
    }
    case Column::k:
    {
        std::sort(_peaks.begin(),_peaks.end(),
              [&](sptrPeak3D p1, sptrPeak3D p2)
              {
                auto hkl1 = p1->getMillerIndices();
                auto hkl2 = p2->getMillerIndices();
                return (hkl1[1]<hkl2[1]);
              }
              );
        break;
    }
    case Column::l:
    {
        std::sort(_peaks.begin(),_peaks.end(),
              [&](sptrPeak3D p1, sptrPeak3D p2)
              {
                auto hkl1 = p1->getMillerIndices();
                auto hkl2 = p2->getMillerIndices();
                return (hkl1[2]<hkl2[2]);
              }
              );
        break;
    }
    case Column::intensity:
    {
        std::sort(_peaks.begin(),
                  _peaks.end(),
                  [&](sptrPeak3D p1, sptrPeak3D p2)
                  {return (p1->getScaledIntensity()/p1->getLorentzFactor()/p1->getTransmission())>(p2->getScaledIntensity()/p2->getLorentzFactor()/p2->getTransmission());});
        break;
    }
    case Column::sigmaIntensity:
    {
        std::sort(_peaks.begin(),
                  _peaks.end(),
                  [&](sptrPeak3D p1, sptrPeak3D p2)
                  {return (p1->getScaledSigma()/p1->getLorentzFactor()/p1->getTransmission())>(p2->getScaledSigma()/p2->getLorentzFactor()/p2->getTransmission());});
        break;
    }
    case Column::transmission:
    {
        std::sort(_peaks.begin(),
                  _peaks.end(),
                  [&](sptrPeak3D p1, sptrPeak3D p2)
                  {return p1->getTransmission()>p2->getTransmission();});
        break;
    }
    case Column::lorentzFactor:
    {
        std::sort(_peaks.begin(),
                  _peaks.end(),
                  [&](sptrPeak3D p1, sptrPeak3D p2)
                  {return p1->getLorentzFactor()>p2->getLorentzFactor();});
        for (auto p : _peaks)
            std::cout<<p->getLorentzFactor()<<std::endl;
        break;
    }
    case Column::numor:
    {
        std::sort(_peaks.begin(),
                  _peaks.end(),
                  [&](sptrPeak3D p1, sptrPeak3D p2)
                  {
                    int numor1=p1->getData()->getMetadata()->getKey<int>("Numor");
                    int numor2=p2->getData()->getMetadata()->getKey<int>("Numor");
                    return (numor1>numor2);});
        break;
    }
    case Column::selected:
    {
        std::sort(_peaks.begin(),
                  _peaks.end(),
                  [&](sptrPeak3D p1, const sptrPeak3D p2)
                  {
                    return (p2->isSelected()<p1->isSelected());
                  });
        for (auto p : _peaks)
            std::cout<<p->isSelected()<<std::endl;
        break;
    }
    case Column::observed:
    {
        std::sort(_peaks.begin(),
                  _peaks.end(),
                  [&](sptrPeak3D p1, const sptrPeak3D p2)
                  {
                    return (p2->isObserved()<p1->isObserved());
                  });
        break;
    }
    case Column::unitCell:
    {
        std::sort(_peaks.begin(),
                  _peaks.end(),
                  [&](sptrPeak3D p1, const sptrPeak3D p2)
                  {
                    auto uc1 = p1->getActiveUnitCell();
                    auto uc2 = p2->getActiveUnitCell();
                    std::string uc1Name = uc1 ? uc1->getName() : "";
                    std::string uc2Name = uc2 ? uc2->getName() : "";
                    return (uc2Name<uc1Name);
                  });
        break;
    }
    }

    if (order == Qt::DescendingOrder)
        std::reverse(_peaks.begin(),_peaks.end());

    emit layoutChanged();
}

bool CollectedPeaksModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if (!indexIsValid(index))
        return false;

    int row = index.row();
    int column = index.column();

    if (role == Qt::CheckStateRole)
    {
        bool state = value.toBool();
        if (column == Column::selected)
            _peaks[row]->setSelected(state);
        else if (column == Column::observed)
            _peaks[row]->setObserved(state);
    }
    else if (role == Qt::EditRole)
    {
        if (column == Column::unitCell)
        {
            if (_cells.empty())
                return false;
            int unitCellIndex = value.toInt();
            sptrUnitCell unitCell = _cells[unitCellIndex];
            _peaks[row]->addUnitCell(unitCell);
        }
    }

    emit dataChanged(index,index);

    return true;
}

bool CollectedPeaksModel::indexIsValid(const QModelIndex& index) const
{
    return index.isValid() && index.row() < _peaks.size();
}

void CollectedPeaksModel::setUnitCells(const SX::Instrument::CellList &cells)
{
    _cells = cells;
}

void CollectedPeaksModel::sortEquivalents()
{
    auto ptrcell=_peaks[0]->getActiveUnitCell();

    // If no unit cell defined for the peak collection, return.
    if (!ptrcell)
    {
        qCritical()<<"No unit cell defined for the peaks";
        return;
    }

    std::sort(_peaks.begin(),
              _peaks.end(),
              [&](sptrPeak3D p1, sptrPeak3D p2)
                {
                    Eigen::Vector3d hkl1=p1->getMillerIndices();
                    Eigen::Vector3d hkl2=p2->getMillerIndices();
                    if (ptrcell->isEquivalent(hkl1[0],hkl1[1],hkl1[2],hkl2[0],hkl2[1],hkl2[2]))
                        return true;
                    else
                        return false;
                }
              );
}

void CollectedPeaksModel::normalizeToMonitor(double factor)
{
    for (sptrPeak3D peak : _peaks)
        peak->setScale(factor/peak->getData()->getMetadata()->getKey<double>("monitor"));
}

void CollectedPeaksModel::writeShelX(const std::string& filename, double tolerance, QModelIndexList indexes)
{
    if (filename.empty())
    {
        qCritical()<<"Empty filename";
        return;
    }

    if (!_peaks.size())
    {
        qCritical()<<"No peaks in the table";
        return;
    }

    auto sptrBasis = _peaks[0]->getActiveUnitCell();
    if (!sptrBasis)
    {
        qCritical()<<"No unit cell defined the peaks. No index can be defined.";
        return;
    }

    if (indexes.isEmpty())
    {
        for (int i=0;i<rowCount();++i)
            indexes << index(i,0);
    }

    std::fstream file(filename,std::ios::out);
    if (!file.is_open())
    {
        qCritical()<<"Error writing to this file, please check write permisions";
        return;
    }

    for (const auto& index  : indexes)
    {
        if (!index.isValid())
            continue;

        sptrPeak3D peak = _peaks[index.row()];

        if (peak->isSelected() && !peak->isMasked())
        {
            const Eigen::RowVector3d& hkl=peak->getMillerIndices();

            if (!(peak->hasIntegerHKL(*sptrBasis,tolerance)))
                continue;

            file << std::fixed;
            file << std::setprecision(0);
            file << std::setw(4);
            file << hkl[0];

            file << std::fixed;
            file << std::setprecision(0);
            file << std::setw(4);
            file <<  hkl[1];

            file << std::fixed;
            file << std::setprecision(0);
            file << std::setw(4);
            file << hkl[2];

            double l=peak->getLorentzFactor();
            double t=peak->getTransmission();
            file << std::fixed << std::setw(8) << std::setprecision(2) << peak->getScaledIntensity()/l/t;
            file << std::fixed << std::setw(8) << std::setprecision(2) << peak->getScaledSigma()/l/t <<std::endl;
        }
    }
    if (file.is_open())
        file.close();
}

void CollectedPeaksModel::writeFullProf(const std::string& filename, double tolerance, QModelIndexList indexes)
{
    if (filename.empty())
    {
        qCritical()<<"Empty filename";
        return;
    }

    if (!_peaks.size())
    {
        qCritical()<<"No peaks in the table";
        return;
    }

    auto sptrBasis = _peaks[0]->getActiveUnitCell();
    if (!sptrBasis)
    {
        qCritical()<<"No unit cell defined the peaks. No index can be defined.";
        return;
    }

    if (indexes.isEmpty())
    {
        for (int i=0;i<rowCount();++i)
            indexes << index(i,0);
    }

    std::fstream file(filename,std::ios::out);

    if (!file.is_open())
    {
        qCritical()<<"Error writing to this file, please check write permisions";
        return;
    }

    file << "TITLE File written by ...\n";
    file << "(3i4,2F14.4,i5,4f8.2)\n";
    double wave=_peaks[0]->getData()->getMetadata()->getKey<double>("wavelength");
    file << std::fixed << std::setw(8) << std::setprecision(3) << wave << " 0 0" << std::endl;
    for (const auto &index : indexes)
    {
        sptrPeak3D peak = _peaks[index.row()];
        if (peak->isSelected() && !peak->isMasked())
        {

            if (!(peak->hasIntegerHKL(*sptrBasis,tolerance)))
                continue;

            const Eigen::RowVector3d& hkl=peak->getMillerIndices();

            file << std::setprecision(0);
            file << std::setw(4);
            file << hkl[0] << std::setw(4) <<  hkl[1] << std::setw(4) << hkl[2];
            double l=peak->getLorentzFactor();
            double t=peak->getTransmission();
            file << std::fixed << std::setw(14) << std::setprecision(4) << peak->getScaledIntensity()/l/t;
            file << std::fixed << std::setw(14) << std::setprecision(4) << peak->getScaledSigma()/l/t;
            file << std::setprecision(0) << std::setw(5) << 1  << std::endl;
        }
    }
    if (file.is_open())
        file.close();
}
