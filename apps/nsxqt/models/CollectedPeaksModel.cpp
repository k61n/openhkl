#include <algorithm>
#include <fstream>
#include <iomanip>
#include <limits>
#include <vector>

#include <QIcon>
#include <QString>

#include <nsxlib/CrystalTypes.h>
#include <nsxlib/DataSet.h>
#include <nsxlib/Detector.h>
#include <nsxlib/Diffractometer.h>
#include <nsxlib/InstrumentState.h>
#include <nsxlib/Logger.h>
#include <nsxlib/MetaData.h>
#include <nsxlib/MillerIndex.h>
#include <nsxlib/Peak3D.h>
#include <nsxlib/PeakFilter.h>
#include <nsxlib/ReciprocalVector.h>
#include <nsxlib/UnitCell.h>

#include "CollectedPeaksModel.h"
#include "ProgressView.h"

struct PeakFactors {

    double gamma;
    double nu;
    double lorentz;
};

static PeakFactors peakFactors(nsx::sptrPeak3D peak)
{
    auto coord = peak->shape().center();
    auto state = peak->data()->interpolatedState(coord[2]);
    auto position = peak->data()->diffractometer()->detector()->pixelPosition(coord[0], coord[1]);

    PeakFactors peak_factors;
    peak_factors.gamma = state.gamma(position);
    peak_factors.nu = state.nu(position);
    peak_factors.lorentz = state.lorentzFactor(coord[0], coord[1]);

    return peak_factors;
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

const nsx::PeakList& CollectedPeaksModel::peaks() const
{
    return _peaks;
}

nsx::PeakList CollectedPeaksModel::peaks(const QModelIndexList &indices) const
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
        return QVariant(section+1);
}

QVariant CollectedPeaksModel::data(const QModelIndex &index, int role) const
{
    if (!indexIsValid(index)) {
        return QVariant();
    }

    Eigen::RowVector3i hkl= {0,0,0};
    double transmissionFactor, scaledIntensity, sigmaScaledIntensity;

    int row = index.row();
    int column = index.column();
    auto cell = _peaks[row]->unitCell();
    if (cell) {
        nsx::MillerIndex miller_index(_peaks[row]->q(), *cell);
        if (miller_index.indexed(cell->indexingTolerance())) {
            hkl = miller_index.rowVector();
        }
    }
    PeakFactors pf = peakFactors(_peaks[row]);
    transmissionFactor = _peaks[row]->transmission();
    scaledIntensity = _peaks[row]->correctedIntensity().value();
    sigmaScaledIntensity = _peaks[row]->correctedIntensity().sigma();

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
            return pf.lorentz;
        case Column::numor:
            return _peaks[row]->data()->metadata()->key<int>("Numor");
        case Column::selected:
            return _peaks[row]->selected();
        case Column::unitCell:
            if (auto unitCell = _peaks[row]->unitCell()) {
                return QString::fromStdString(unitCell->name());
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
            return _peaks[row]->selected();
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
            auto cell1 = p1->unitCell();
            auto cell2 = p2->unitCell();
            nsx::MillerIndex miller_index1(p1->q(), *cell1);
            nsx::MillerIndex miller_index2(p2->q(), *cell2);
            return (miller_index1[0]<miller_index2[0]);
        };
        break;
    case Column::k:
        compareFn = [&](nsx::sptrPeak3D p1, nsx::sptrPeak3D p2) {
            auto cell1 = p1->unitCell();
            auto cell2 = p2->unitCell();
            nsx::MillerIndex miller_index1(p1->q(), *cell1);
            nsx::MillerIndex miller_index2(p2->q(), *cell2);
            return (miller_index1[1]<miller_index2[1]);
        };
        break;
    case Column::l:
        compareFn = [](nsx::sptrPeak3D p1, nsx::sptrPeak3D p2) {
            auto cell1 = p1->unitCell();
            auto cell2 = p2->unitCell();
            nsx::MillerIndex miller_index1(p1->q(), *cell1);
            nsx::MillerIndex miller_index2(p2->q(), *cell2);
            return (miller_index1[2]<miller_index2[2]);
        };
        break;
    case  Column::intensity:
        compareFn = [](nsx::sptrPeak3D p1, nsx::sptrPeak3D p2) {
            return (p1->correctedIntensity().value())
                    > (p2->correctedIntensity().value());
        };
        break;
    case Column::sigmaIntensity:
        compareFn = [](nsx::sptrPeak3D p1, nsx::sptrPeak3D p2) {
            return (p1->correctedIntensity().sigma())
                    > (p2->correctedIntensity().sigma());
        };
        break;
    case Column::i_over_sigmai:
        compareFn = [](nsx::sptrPeak3D p1, nsx::sptrPeak3D p2) {
            return (p1->correctedIntensity().value()/p1->correctedIntensity().sigma())
                    > (p2->correctedIntensity().value()/p2->correctedIntensity().sigma());
        };
        break;
    case Column::transmission:
        compareFn = [&](nsx::sptrPeak3D p1, nsx::sptrPeak3D p2) {
            return p1->transmission()>p2->transmission();
        };
        break;
    case Column::lorentzFactor:
        compareFn = [&](nsx::sptrPeak3D p1, nsx::sptrPeak3D p2) {
            auto pf1 = peakFactors(p1);
            auto pf2 = peakFactors(p2);
            return pf1.lorentz > pf2.lorentz;
        };
        break;
    case Column::numor:
        compareFn = [&](nsx::sptrPeak3D p1, nsx::sptrPeak3D p2) {
            int numor1=p1->data()->metadata()->key<int>("Numor");
            int numor2=p2->data()->metadata()->key<int>("Numor");
            return (numor1>numor2);
        };
        break;
    case Column::selected:
        compareFn = [&](nsx::sptrPeak3D p1, const nsx::sptrPeak3D p2) {
            return (p2->selected()<p1->selected());
        };
        break;
    case Column::unitCell:
        compareFn = [&](nsx::sptrPeak3D p1, const nsx::sptrPeak3D p2) {
            auto uc1 = p1->unitCell();
            auto uc2 = p2->unitCell();
            std::string uc1Name = uc1 ? uc1->name() : "";
            std::string uc2Name = uc2 ? uc2->name() : "";
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
        if (column == Column::selected) {
            _peaks[row]->setSelected(state);
        }
    }
    emit dataChanged(index,index);
    return true;
}

bool CollectedPeaksModel::indexIsValid(const QModelIndex& index) const
{
    return index.isValid() && (index.row() < static_cast<int>(_peaks.size()));
}

void CollectedPeaksModel::sortEquivalents()
{
    // todo: investigate this method. Likely incorrect if there are multiple unit cells.
    auto cell=_peaks[0]->unitCell();

    // If no unit cell defined for the peak collection, return.
    if (!cell) {
        nsx::error() << "No unit cell defined for the peaks";
        return;
    }

    std::sort(_peaks.begin(), _peaks.end(), [&](nsx::sptrPeak3D p1, nsx::sptrPeak3D p2) {
        nsx::MillerIndex miller_index1(p1->q(), *cell);
        nsx::MillerIndex miller_index2(p2->q(), *cell);
        return cell->spaceGroup().isEquivalent(miller_index1,miller_index2);
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
        peak->setUnitCell(unitCell);
    }
    emit layoutChanged();
    emit unitCellUpdated();
}

void CollectedPeaksModel::normalizeToMonitor(double factor)
{
    for (auto&& peak : _peaks) {
        peak->setScale(factor/peak->data()->metadata()->key<double>("monitor"));
    }
}

QModelIndexList CollectedPeaksModel::getUnindexedPeaks()
{
    QModelIndexList list;

    for (int i=0; i<rowCount(); ++i) {
        auto peak = _peaks[i];
        if (!peak->unitCell()) {
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
        if (peak->selected()) {
            list.append(index(i,0));
        }
    }
    return list;
}

nsx::sptrExperiment CollectedPeaksModel::getExperiment()
{
    return _experiment;
}
