#include <algorithm>
#include <fstream>
#include <iomanip>
#include <limits>

#include <QDebug>
#include <QIcon>
#include <QString>

#include <nsxlib/crystal/Peak3D.h>
#include <nsxlib/crystal/UnitCell.h>
#include <nsxlib/data/DataSet.h>
#include <nsxlib/data/MetaData.h>

#include "CollectedPeaksModel.h"
#include "views/ProgressView.h"

CollectedPeaksModel::CollectedPeaksModel(nsx::sptrExperiment experiment, QObject *parent)
: QAbstractTableModel(parent),
  _experiment(std::move(experiment))
{
}

CollectedPeaksModel::CollectedPeaksModel(nsx::sptrExperiment experiment, const nsx::PeakList &peaks, QObject *parent)
: QAbstractTableModel(parent),
  _experiment(std::move(experiment)),
  _peaks(peaks)
{
}

void CollectedPeaksModel::addPeak(const nsx::sptrPeak3D& peak)
{
    auto it=std::find(_peaks.begin(),_peaks.end(),peak);
    if (it!=_peaks.end()) {
        return;
    }
    _peaks.push_back(peak);
}

void CollectedPeaksModel::setPeaks(const nsx::PeakList& peaks)
{
    _peaks = peaks;
}

void CollectedPeaksModel::setPeaks(const nsx::DataList &data)
{
    _peaks.clear();

    for (auto&& ptr: data) {
        // Add peaks present in this numor to the model
        for (auto&& peak: ptr->getPeaks()) {
            _peaks.push_back(peak);
        }
    }
}

const nsx::PeakList& CollectedPeaksModel::getPeaks() const
{
    return _peaks;
}

nsx::PeakList CollectedPeaksModel::getPeaks(const QModelIndexList &indices) const
{
    nsx::PeakList peaks;
    peaks.reserve(indices.count());
    for (auto&& index: indices) {
        peaks.push_back(_peaks[index.row()]);
    }
    return peaks;
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
    if (!indexIsValid(index)) {
        return Qt::ItemIsEnabled;
    }

    int column = index.column();

    if (column == Column::selected) {
        return QAbstractTableModel::flags(index) | Qt::ItemIsEnabled | Qt::ItemIsUserCheckable;
    }
    if (column == Column::unitCell) {
        return QAbstractTableModel::flags(index) | Qt::ItemIsEditable;
    }
    return QAbstractTableModel::flags(index);
}

QVariant CollectedPeaksModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole) {
        return QVariant();
    }
    if (orientation == Qt::Horizontal) {
        switch(section) {
        case Column::h:
            return QString("h");
        case Column::k:
            return QString("k");
        case Column::l:
            return QString("l");
        case Column::intensity:
            return QString("intensity");
        case Column::sigmaIntensity:
            return QString(QChar(0x03C3))+"(intensity)";
        case Column::transmission:
            return QString("transmission");
        case Column::lorentzFactor:
            return QString("lorentz factor");
        case Column::numor:
            return QString("numor");
        case Column::selected:
            return QString("valid");
        case Column::unitCell:
            return QString("unit cell");
        default:
            return QVariant();
        }
    }
    return QString("Peak");
}

QVariant CollectedPeaksModel::data(const QModelIndex &index, int role) const
{
    if (!indexIsValid(index)) {
        return QVariant();
    }

    Eigen::RowVector3d hkl;
    double lorentzFactor, transmissionFactor, scaledIntensity, sigmaScaledIntensity;

    int row = index.row();
    int column = index.column();

    switch (role) {

    case Qt::DisplayRole:
        _peaks[row]->getMillerIndices(hkl, true);
        lorentzFactor = _peaks[row]->getLorentzFactor();
        transmissionFactor = _peaks[row]->getTransmission();
        scaledIntensity=_peaks[row]->getCorrectedIntensity().getValue();
        sigmaScaledIntensity=_peaks[row]->getCorrectedIntensity().getSigma();

        switch (column) {
        case Column::h:
            return hkl(0);
        case Column::k:
            return hkl(1);
        case Column::l:
            return hkl(2);
        case Column::intensity:
            return scaledIntensity;
        case Column::sigmaIntensity:
            return sigmaScaledIntensity;
        case Column::transmission:
            return transmissionFactor;
        case Column::lorentzFactor:
            return lorentzFactor;
        case Column::numor:
            return _peaks[row]->getData()->getMetadata()->getKey<int>("Numor");
        case Column::selected:
            return _peaks[row]->isSelected();
        case Column::unitCell:
            if (auto unitCell = _peaks[row]->getActiveUnitCell()) {
                return QString::fromStdString(unitCell->getName());
            }
            else {
                return QString("not set");
            }
        }
    case Qt::ToolTipRole:
        switch (column) {
            case Column::h:
                _peaks[row]->getMillerIndices(hkl, false);
                return hkl[0];
            case Column::k:
                _peaks[row]->getMillerIndices(hkl, false);
                return hkl[1];
            case Column::l:
                _peaks[row]->getMillerIndices(hkl, false);
                return hkl[2];
        }
    case Qt::CheckStateRole:
        if (column == Column::selected) {
            return _peaks[row]->isSelected();
        }
        break;
    case Qt::UserRole:
        if (column == Column::unitCell) {
            QStringList cellNames;
            for (auto&& cell : _cells) {
                cellNames.append(QString::fromStdString(cell->getName()));
            }
            return cellNames;
        }
        break;
    }
    return QVariant::Invalid;
}

void CollectedPeaksModel::sort(int column, Qt::SortOrder order)
{
    std::function<bool(nsx::sptrPeak3D, nsx::sptrPeak3D)> compareFn = [](nsx::sptrPeak3D, nsx::sptrPeak3D) { return false; };

    switch (column) {
    case Column::h:
        compareFn = [&](nsx::sptrPeak3D p1, nsx::sptrPeak3D p2) {
            Eigen::RowVector3d hkl1,hkl2;
            p1->getMillerIndices(hkl1,true);
            p2->getMillerIndices(hkl2,true);
            return (hkl1[0]<hkl2[0]);
        };
        break;
    case Column::k:
        compareFn = [&](nsx::sptrPeak3D p1, nsx::sptrPeak3D p2) {
            Eigen::RowVector3d hkl1,hkl2;
            p1->getMillerIndices(hkl1,true);
            p2->getMillerIndices(hkl2,true);
            return (hkl1[1]<hkl2[1]);
        };
        break;
    case Column::l:
        compareFn = [&](nsx::sptrPeak3D p1, nsx::sptrPeak3D p2) {
            Eigen::RowVector3d hkl1,hkl2;
            p1->getMillerIndices(hkl1,true);
            p2->getMillerIndices(hkl2,true);
            return (hkl1[2]<hkl2[2]);
        };
        break;
    case  Column::intensity:
        compareFn = [&](nsx::sptrPeak3D p1, nsx::sptrPeak3D p2) {
            return (p1->getCorrectedIntensity().getValue())
                    > (p2->getCorrectedIntensity().getValue());
        };
        break;
    case Column::sigmaIntensity:
        compareFn = [&](nsx::sptrPeak3D p1, nsx::sptrPeak3D p2) {
            return (p1->getCorrectedIntensity().getSigma())
                    > (p2->getCorrectedIntensity().getSigma());
        };
        break;
    case Column::transmission:
        compareFn = [&](nsx::sptrPeak3D p1, nsx::sptrPeak3D p2) {
            return p1->getTransmission()>p2->getTransmission();
        };
        break;
    case Column::lorentzFactor:
        compareFn = [&](nsx::sptrPeak3D p1, nsx::sptrPeak3D p2) {
            return p1->getLorentzFactor()>p2->getLorentzFactor();
        };
        break;
    case Column::numor:
        compareFn = [&](nsx::sptrPeak3D p1, nsx::sptrPeak3D p2) {
            int numor1=p1->getData()->getMetadata()->getKey<int>("Numor");
            int numor2=p2->getData()->getMetadata()->getKey<int>("Numor");
            return (numor1>numor2);
        };
        break;
    case Column::selected:
        compareFn = [&](nsx::sptrPeak3D p1, const nsx::sptrPeak3D p2) {
            return (p2->isSelected()<p1->isSelected());
        };
        break;
    case Column::unitCell:
        compareFn = [&](nsx::sptrPeak3D p1, const nsx::sptrPeak3D p2) {
            auto uc1 = p1->getActiveUnitCell();
            auto uc2 = p2->getActiveUnitCell();
            std::string uc1Name = uc1 ? uc1->getName() : "";
            std::string uc2Name = uc2 ? uc2->getName() : "";
            return (uc2Name<uc1Name);
        };
        break;
    }
    std::sort(_peaks.begin(), _peaks.end(), compareFn);

    if (order == Qt::DescendingOrder) {
        std::reverse(_peaks.begin(),_peaks.end());
    }
    emit layoutChanged();
}

bool CollectedPeaksModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if (!indexIsValid(index)) {
        return false;
    }

    int row = index.row();
    int column = index.column();

    if (role == Qt::CheckStateRole) {
        bool state = value.toBool();
        if (column == Column::selected)
            _peaks[row]->setSelected(state);
    }
    else if (role == Qt::EditRole) {
        if (column == Column::unitCell) {
            if (_cells.empty())
                return false;
            int unitCellIndex = value.toInt();
            auto unitCell = _cells[unitCellIndex];
            _peaks[row]->addUnitCell(unitCell,true);
        }
    }
    emit dataChanged(index,index);
    emit updateFrame();
    return true;
}

bool CollectedPeaksModel::indexIsValid(const QModelIndex& index) const
{
    return index.isValid() && (index.row() < _peaks.size());
}

void CollectedPeaksModel::setUnitCells(const nsx::UnitCellList &cells)
{
    _cells = cells;
}

void CollectedPeaksModel::sortEquivalents()
{
    auto ptrcell=_peaks[0]->getActiveUnitCell();

    // If no unit cell defined for the peak collection, return.
    if (ptrcell == nullptr) {
        qCritical() << "No unit cell defined for the peaks";
        return;
    }

    std::sort(_peaks.begin(), _peaks.end(), [&](nsx::sptrPeak3D p1, nsx::sptrPeak3D p2) {
        Eigen::RowVector3d hkl1,hkl2;
        p1->getMillerIndices(hkl1,true);
        p2->getMillerIndices(hkl2,true);
        return ptrcell->isEquivalent(hkl1[0],hkl1[1],hkl1[2],hkl2[0],hkl2[1],hkl2[2]);
    });
}

void CollectedPeaksModel::setUnitCell(const nsx::sptrUnitCell& unitCell, QModelIndexList selectedPeaks)
{
    if (selectedPeaks.isEmpty()) {
        for (int i=0;i<rowCount();++i) {
            selectedPeaks << index(i,0);
        }
    }
    for (auto&& index : selectedPeaks) {
        auto peak = _peaks[index.row()];
        peak->addUnitCell(unitCell,true);
    }
    emit layoutChanged();
    emit unitCellUpdated();
}

void CollectedPeaksModel::normalizeToMonitor(double factor)
{
    for (auto&& peak : _peaks) {
        peak->setScale(factor/peak->getData()->getMetadata()->getKey<double>("monitor"));
    }
}

void CollectedPeaksModel::writeShelX(const std::string& filename, QModelIndexList indices)
{
    if (filename.empty()) {
        qCritical()<<"Empty filename";
        return;
    }
    if (_peaks.empty()) {
        qCritical()<<"No peaks in the table";
        return;
    }
    if (indices.isEmpty()) {
        for (int i=0;i<rowCount();++i) {
            indices << index(i,0);
        }
    }

    std::fstream file(filename,std::ios::out);
    if (!file.is_open()) {
        qCritical()<<"Error writing to this file, please check write permisions";
        return;
    }

    for (auto&& index  : indices) {
        if (!index.isValid()) {
            continue;
        }
        auto peak = _peaks[index.row()];
        auto basis = peak->getActiveUnitCell();

        if (basis == nullptr) {
            qCritical()<<QString("No unit cell defined for peak %1. It will not be written to ShelX file").arg(index.row()+1);
            continue;
        }

        if (peak->isSelected() && !peak->isMasked()) {
            Eigen::RowVector3d hkl;
            bool success = peak->getMillerIndices(hkl,true);

            if (!success) {
                continue;
            }

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

            file << std::fixed << std::setw(8) << std::setprecision(2) << peak->getCorrectedIntensity().getValue();
            file << std::fixed << std::setw(8) << std::setprecision(2) << peak->getCorrectedIntensity().getSigma() <<std::endl;
        }
    }
    if (file.is_open()) {
        file.close();
    }
}

void CollectedPeaksModel::writeFullProf(const std::string& filename, QModelIndexList indices)
{
    if (filename.empty()) {
        qCritical()<<"Empty filename";
        return;
    }
    if (_peaks.empty()) {
        qCritical()<<"No peaks in the table";
        return;
    }
    if (indices.isEmpty()) {
        for (int i=0;i<rowCount();++i) {
            indices << index(i,0);
        }
    }
    std::fstream file(filename,std::ios::out);

    if (!file.is_open()) {
        qCritical()<<"Error writing to this file, please check write permisions";
        return;
    }

    file << "TITLE File written by ...\n";
    file << "(3i4,2F14.4,i5,4f8.2)\n";
    double wave=_peaks[0]->getData()->getMetadata()->getKey<double>("wavelength");
    file << std::fixed << std::setw(8) << std::setprecision(3) << wave << " 0 0" << std::endl;

    for (const auto &index : indices) {
        auto peak = _peaks[index.row()];
        auto basis = peak->getActiveUnitCell();
        if (!basis) {
            qCritical()<<QString("No unit cell defined for peak %1. It will not be written to FullProf file").arg(index.row()+1);
            continue;
        }
        if (peak->isSelected() && !peak->isMasked()) {
            Eigen::RowVector3d hkl;
            bool success = peak->getMillerIndices(hkl,true);
            if (!success) {
                continue;
            }
            file << std::setprecision(0);
            file << std::setw(4);
            file << hkl[0] << std::setw(4) <<  hkl[1] << std::setw(4) << hkl[2];
            file << std::fixed << std::setw(14) << std::setprecision(4) << peak->getCorrectedIntensity().getValue();
            file << std::fixed << std::setw(14) << std::setprecision(4) << peak->getCorrectedIntensity().getSigma();
            file << std::setprecision(0) << std::setw(5) << 1  << std::endl;
        }
    }
    if (file.is_open()) {
        file.close();
    }
}

QModelIndexList CollectedPeaksModel::getUnindexedPeaks()
{
    QModelIndexList list;

    for (int i=0; i<rowCount(); ++i) {
        auto peak = _peaks[i];
        if (!peak->hasUnitCells()) {
            list.append(index(i,0));
        }
    }
    return list;
}

QModelIndexList CollectedPeaksModel::getValidPeaks()
{
    QModelIndexList list;

    for (int i=0; i<rowCount(); ++i) {
        auto peak = _peaks[i];
        if (peak->isSelected()) {
            list.append(index(i,0));
        }
    }
    return list;
}

nsx::sptrExperiment CollectedPeaksModel::getExperiment()
{
    return _experiment;
}
