#include <algorithm>
#include <fstream>
#include <iomanip>
#include <limits>

#include <QIcon>
#include <QString>

#include <nsxlib/DataSet.h>

#include <nsxlib/Detector.h>
#include <nsxlib/Diffractometer.h>
#include <nsxlib/InstrumentState.h>
#include <nsxlib/Logger.h>
#include <nsxlib/MetaData.h>
#include <nsxlib/Peak3D.h>
#include <nsxlib/ReciprocalVector.h>
#include <nsxlib/UnitCell.h>

#include "CollectedPeaksModel.h"
#include "ProgressView.h"

static void peakFactors(double& gamma, double& nu, double& lorentz, nsx::sptrPeak3D peak)
{
    auto coord = peak->getShape().center();
    auto state = peak->data()->getInterpolatedState(coord[2]);
    auto position = peak->data()->getDiffractometer()->getDetector()->getPos(coord[0], coord[1]);
    lorentz = state.getLorentzFactor(position);
    state.getGammaNu(gamma, nu, position);
}

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
        case Column::i_over_sigmai:
            return "intensity/" + QString(QChar(0x03C3))+"(intensity)";
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
    } else
        return QVariant();
}

QVariant CollectedPeaksModel::data(const QModelIndex &index, int role) const
{
    if (!indexIsValid(index)) {
        return QVariant();
    }

    Eigen::RowVector3d hkl= {0,0,0};
    double lorentzFactor, transmissionFactor, scaledIntensity, sigmaScaledIntensity;

    int row = index.row();
    int column = index.column();
    if (auto cell = _peaks[row]->getActiveUnitCell()) {
        bool success = cell->getMillerIndices(_peaks[row]->getQ(), hkl, true);
    }
    auto c = _peaks[row]->getShape().center();
    double gamma, nu;
    peakFactors(gamma, nu, lorentzFactor, _peaks[row]);
    transmissionFactor = _peaks[row]->getTransmission();
    scaledIntensity = _peaks[row]->getCorrectedIntensity().value();
    sigmaScaledIntensity = _peaks[row]->getCorrectedIntensity().sigma();

    switch (role) {

    case Qt::DisplayRole:        

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
        case Column::i_over_sigmai:
            return scaledIntensity/sigmaScaledIntensity;
        case Column::transmission:
            return transmissionFactor;
        case Column::lorentzFactor:
            return lorentzFactor;
        case Column::numor:
            return _peaks[row]->data()->getMetadata()->getKey<int>("Numor");
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
        break;
    case Qt::ToolTipRole:
        switch (column) {
            case Column::h:                
                return hkl[0];
            case Column::k:                
                return hkl[1];
            case Column::l:                
                return hkl[2];
        }
        break;
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
            auto cell1 = p1->getActiveUnitCell();
            auto cell2 = p2->getActiveUnitCell();
            cell1->getMillerIndices(p1->getQ(), hkl1, true);
            cell2->getMillerIndices(p2->getQ(), hkl2, true);
            return (hkl1[0]<hkl2[0]);
        };
        break;
    case Column::k:
        compareFn = [&](nsx::sptrPeak3D p1, nsx::sptrPeak3D p2) {
            Eigen::RowVector3d hkl1,hkl2;
            auto cell1 = p1->getActiveUnitCell();
            auto cell2 = p2->getActiveUnitCell();
            cell1->getMillerIndices(p1->getQ(), hkl1, true);
            cell2->getMillerIndices(p2->getQ(), hkl2, true);
            return (hkl1[1]<hkl2[1]);
        };
        break;
    case Column::l:
        compareFn = [](nsx::sptrPeak3D p1, nsx::sptrPeak3D p2) {
            Eigen::RowVector3d hkl1,hkl2;
            auto cell1 = p1->getActiveUnitCell();
            auto cell2 = p2->getActiveUnitCell();
            cell1->getMillerIndices(p1->getQ(), hkl1, true);
            cell2->getMillerIndices(p2->getQ(), hkl2, true);
            return (hkl1[2]<hkl2[2]);
        };
        break;
    case  Column::intensity:
        compareFn = [](nsx::sptrPeak3D p1, nsx::sptrPeak3D p2) {
            return (p1->getCorrectedIntensity().value())
                    > (p2->getCorrectedIntensity().value());
        };
        break;
    case Column::sigmaIntensity:
        compareFn = [](nsx::sptrPeak3D p1, nsx::sptrPeak3D p2) {
            return (p1->getCorrectedIntensity().sigma())
                    > (p2->getCorrectedIntensity().sigma());
        };
        break;
    case Column::i_over_sigmai:
        compareFn = [](nsx::sptrPeak3D p1, nsx::sptrPeak3D p2) {
            return (p1->getCorrectedIntensity().value()/p1->getCorrectedIntensity().sigma())
                    > (p2->getCorrectedIntensity().value()/p2->getCorrectedIntensity().sigma());
        };
        break;
    case Column::transmission:
        compareFn = [&](nsx::sptrPeak3D p1, nsx::sptrPeak3D p2) {
            return p1->getTransmission()>p2->getTransmission();
        };
        break;
    case Column::lorentzFactor:
        compareFn = [&](nsx::sptrPeak3D p1, nsx::sptrPeak3D p2) {
            double g, n, lor1, lor2;
            peakFactors(g, n, lor1, p1);
            peakFactors(g, n, lor2, p2);
            return lor1 > lor2;
        };
        break;
    case Column::numor:
        compareFn = [&](nsx::sptrPeak3D p1, nsx::sptrPeak3D p2) {
            int numor1=p1->data()->getMetadata()->getKey<int>("Numor");
            int numor2=p2->data()->getMetadata()->getKey<int>("Numor");
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
    // todo: investigate this method. Likely incorrect if there are multiple unit cells.
    auto ptrcell=_peaks[0]->getActiveUnitCell();

    // If no unit cell defined for the peak collection, return.
    if (ptrcell == nullptr) {
        nsx::error() << "No unit cell defined for the peaks";
        return;
    }

    std::sort(_peaks.begin(), _peaks.end(), [&](nsx::sptrPeak3D p1, nsx::sptrPeak3D p2) {
        Eigen::RowVector3d hkl1,hkl2;
        ptrcell->getMillerIndices(p1->getQ(), hkl1, true);
        ptrcell->getMillerIndices(p2->getQ(), hkl2, true);
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
        peak->setScale(factor/peak->data()->getMetadata()->getKey<double>("monitor"));
    }
}

void CollectedPeaksModel::writeShelX(const std::string& filename, QModelIndexList indices)
{
    if (filename.empty()) {
        nsx::error()<<"Empty filename";
        return;
    }
    if (_peaks.empty()) {
        nsx::error()<<"No peaks in the table";
        return;
    }
    if (indices.isEmpty()) {
        for (int i=0;i<rowCount();++i) {
            indices << index(i,0);
        }
    }

    std::fstream file(filename,std::ios::out);
    if (!file.is_open()) {
        nsx::error()<<"Error writing to this file, please check write permisions";
        return;
    }

    for (auto&& index  : indices) {
        if (!index.isValid()) {
            continue;
        }
        auto peak = _peaks[index.row()];
        auto basis = peak->getActiveUnitCell();

        if (basis == nullptr) {
            nsx::error()<<"No unit cell defined for peak " << index.row()+1 << ". It will not be written to ShelX file";
            continue;
        }

        if (peak->isSelected() && !peak->isMasked()) {
            Eigen::RowVector3d hkl;
            bool success = basis->getMillerIndices(peak->getQ(), hkl, true);

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

            file << std::fixed << std::setw(8) << std::setprecision(2) << peak->getCorrectedIntensity().value();
            file << std::fixed << std::setw(8) << std::setprecision(2) << peak->getCorrectedIntensity().sigma() <<std::endl;
        }
    }
    if (file.is_open()) {
        file.close();
    }
}

void CollectedPeaksModel::writeFullProf(const std::string& filename, QModelIndexList indices)
{
    if (filename.empty()) {
        nsx::error()<<"Empty filename";
        return;
    }
    if (_peaks.empty()) {
        nsx::error()<<"No peaks in the table";
        return;
    }
    if (indices.isEmpty()) {
        for (int i=0;i<rowCount();++i) {
            indices << index(i,0);
        }
    }
    std::fstream file(filename,std::ios::out);

    if (!file.is_open()) {
        nsx::error()<<"Error writing to this file, please check write permisions";
        return;
    }

    file << "TITLE File written by ...\n";
    file << "(3i4,2F14.4,i5,4f8.2)\n";
    double wave=_peaks[0]->data()->getMetadata()->getKey<double>("wavelength");
    file << std::fixed << std::setw(8) << std::setprecision(3) << wave << " 0 0" << std::endl;

    for (const auto &index : indices) {
        auto peak = _peaks[index.row()];
        auto basis = peak->getActiveUnitCell();
        if (!basis) {
            nsx::error()<<"No unit cell defined for peak " << index.row()+1 << ". It will not be written to FullProf file";
            continue;
        }
        if (peak->isSelected() && !peak->isMasked()) {
            Eigen::RowVector3d hkl;
            bool success = basis->getMillerIndices(peak->getQ(), hkl,true);
            if (!success) {
                continue;
            }
            file << std::setprecision(0);
            file << std::setw(4);
            file << hkl[0] << std::setw(4) <<  hkl[1] << std::setw(4) << hkl[2];
            file << std::fixed << std::setw(14) << std::setprecision(4) << peak->getCorrectedIntensity().value();
            file << std::fixed << std::setw(14) << std::setprecision(4) << peak->getCorrectedIntensity().sigma();
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
