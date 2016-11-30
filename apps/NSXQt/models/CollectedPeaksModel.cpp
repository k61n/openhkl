#include <algorithm>
#include <iostream>
#include <limits>

#include <QString>

#include "CollectedPeaksModel.h"

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

int CollectedPeaksModel::rowCount(const QModelIndex &parent) const
{
    return _peaks.size();
}

int CollectedPeaksModel::columnCount(const QModelIndex &parent) const
{
    return Column::count;
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
        default:
            return QVariant();
        }
    else
        return QString("Peak %1").arg(section);
}

QVariant CollectedPeaksModel::data(const QModelIndex &index, int role) const
{

    if (!indexIsValid(index))
        return QVariant();

    if (role==Qt::DisplayRole || role==Qt::EditRole)
    {
        int row = index.row();
        int column = index.column();
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
            double lorentzFactor = _peaks[row]->getLorentzFactor();
            return lorentzFactor;
        }
        case Column::numor:
        {
            return QString("numor");
        }
        case Column::selected:
        {
            return QString("selected");
        }
        case Column::observed:
        {
            return QString("observed");
        }
        default:
        {
            return QVariant();
        }
        }
    }
    else
        return QVariant();
}

bool CollectedPeaksModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if (indexIsValid(index) && role == Qt::EditRole)
    {
        emit dataChanged(index,index);
        return true;
    }

    return false;
}

bool CollectedPeaksModel::indexIsValid(const QModelIndex& index) const
{
    return index.isValid() && index.row() < _peaks.size();
}

double CollectedPeaksModel::getMinIntensity() const
{
    if (_peaks.size() == 0)
        return 0.0;

    double miniSigmaScaledIntensity = std::numeric_limits<double>::infinity();
    for (auto&& p : _peaks)
    {
        double lorentzFactor = p->getLorentzFactor();
        double transmissionFactor = p->getTransmission();
        double sigmaScaledIntensity = p->getScaledSigma()/lorentzFactor/transmissionFactor;

        if (sigmaScaledIntensity < miniSigmaScaledIntensity)
            miniSigmaScaledIntensity = sigmaScaledIntensity;
    }

    return miniSigmaScaledIntensity;
}

double CollectedPeaksModel::getMinSigmaIntensity() const
{
    if (_peaks.size() == 0)
        return 0.0;

    double miniScaledIntensity = std::numeric_limits<double>::infinity();
    for (auto&& p : _peaks)
    {
        double lorentzFactor = p->getLorentzFactor();
        double transmissionFactor = p->getTransmission();
        double scaledIntensity = p->getScaledIntensity()/lorentzFactor/transmissionFactor;

        if (scaledIntensity < miniScaledIntensity)
            miniScaledIntensity = scaledIntensity;
    }

    return miniScaledIntensity;
}
