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
#include "SessionModel.h"

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

CollectedPeaksModel::CollectedPeaksModel(SessionModel* session, nsx::sptrExperiment experiment, QObject *parent)
: QAbstractTableModel(parent),
  _experiment(std::move(experiment)),
  _peaks()
{
    setSession(session);
}

CollectedPeaksModel::CollectedPeaksModel(SessionModel* session, nsx::sptrExperiment experiment, const nsx::PeakList &peaks, QObject *parent)
: QAbstractTableModel(parent),
  _experiment(std::move(experiment)),
  _peaks(peaks)
{
    setSession(session);
}

SessionModel* CollectedPeaksModel::session()
{
    return _session;
}

void CollectedPeaksModel::setSession(SessionModel* session)
{
    _session = session;
    connect(this, &CollectedPeaksModel::signalSelectedPeakChanged,[this](nsx::sptrPeak3D peak){emit _session->signalSelectedPeakChanged(peak);});
    connect(_session,SIGNAL(signalEnabledPeakChanged(nsx::sptrPeak3D)),this,SLOT(slotChangeEnabledPeak(nsx::sptrPeak3D)));
    connect(_session,SIGNAL(signalMaskedPeaksChanged(const nsx::PeakList&)),this,SLOT(slotChangeMaskedPeaks(const nsx::PeakList&)));
    connect(_session,SIGNAL(signalUnitCellRemoved(nsx::sptrUnitCell)),this,SLOT(slotRemoveUnitCell(nsx::sptrUnitCell)));
}

void CollectedPeaksModel::slotRemoveUnitCell(const nsx::sptrUnitCell unit_cell)
{
    Q_UNUSED(unit_cell)

    QModelIndex topleft_index = index(0,0);
    QModelIndex bottomright_index = index(rowCount(QModelIndex())-1,columnCount(QModelIndex())-1);

    emit dataChanged(topleft_index,bottomright_index);
}

void CollectedPeaksModel::slotChangeMaskedPeaks(const nsx::PeakList& peaks)
{
    for (auto peak : peaks) {

        auto it = std::find(_peaks.begin(),_peaks.end(),peak);
        if (it == _peaks.end()) {
            continue;
        }

        int row = std::distance(_peaks.begin(),it);

        QModelIndex topleft_index = index(row,0);
        QModelIndex bottomright_index = index(row,columnCount(QModelIndex())-1);

        emit dataChanged(topleft_index,bottomright_index);
    }
}

void CollectedPeaksModel::setPeaks(const nsx::PeakList& peaks) {
    reset();
    _peaks = peaks;

    QModelIndex topleft_index = index(0,0);
    QModelIndex bottomright_index = index(rowCount(QModelIndex())-1,columnCount(QModelIndex())-1);

    emit dataChanged(topleft_index,bottomright_index);
}

const nsx::PeakList& CollectedPeaksModel::peaks() const
{
    return _peaks;
}

void CollectedPeaksModel::slotChangeEnabledPeak(nsx::sptrPeak3D peak)
{
    auto it = std::find(_peaks.begin(),_peaks.end(),peak);
    if (it == _peaks.end()) {
        return;
    }

    int row = std::distance(_peaks.begin(),it);

    QModelIndex topleft_index = index(row,0);
    QModelIndex bottomright_index = index(row,columnCount(QModelIndex())-1);

    emit dataChanged(topleft_index,bottomright_index);
}


int CollectedPeaksModel::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);

    return _peaks.size();
}

int CollectedPeaksModel::columnCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);

    return Column::count;
}

void CollectedPeaksModel::reset()
{
    beginResetModel();
    _peaks.clear();
    endResetModel();
}

Qt::ItemFlags CollectedPeaksModel::flags(const QModelIndex &index) const
{
    if (!indexIsValid(index)) {
        return Qt::ItemIsEnabled;
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
        case Column::h: {
            return QString("h");
        }
        case Column::k: {
            return QString("k");
        }
        case Column::l: {
            return QString("l");
        }
        case Column::px: {
            return QString("pixel x");
        }
        case Column::py: {
            return QString("pixel y");
        }
        case Column::frame: {
            return QString("frame");
        }
        case Column::intensity: {
            return QString("intensity");
        }
        case Column::sigmaIntensity: {
            return QString(QChar(0x03C3))+"(intensity)";
        }
        case Column::numor: {
            return QString("numor");
        }
        case Column::unitCell: {
            return QString("unit cell");
        }
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

    int row = index.row();
    int column = index.column();
    auto cell = _peaks[row]->unitCell();
    if (cell) {
        nsx::MillerIndex miller_index(_peaks[row]->q(), *cell);
        if (miller_index.indexed(cell->indexingTolerance())) {
            hkl = miller_index.rowVector();
        }
    }

    double intensity = _peaks[row]->correctedIntensity().value();
    double sigma_intensity = _peaks[row]->correctedIntensity().sigma();

    auto peak_center = _peaks[row]->shape().center();

    switch (role) {

    case Qt::DisplayRole:        

        switch (column) {
        case Column::h: {
            return hkl(0);
        }
        case Column::k: {
            return hkl(1);
        }
        case Column::l: {
            return hkl(2);
        }
        case Column::px: {
            return peak_center(0);
        }
        case Column::py: {
            return peak_center(1);
        }
        case Column::frame: {
            return peak_center(2);
        }
        case Column::intensity: {
            return intensity;
        }
        case Column::sigmaIntensity: {
            return sigma_intensity;
        }
        case Column::numor: {
            return _peaks[row]->data()->metadata()->key<int>("Numor");
        }
        case Column::unitCell:
            auto unit_cell = _peaks[row]->unitCell();
            if (unit_cell) {
                return QString::fromStdString(unit_cell->name());
            }
            else {
                return QString("not set");
            }
        }
        break;
    case Qt::ForegroundRole: {

        if (_peaks[row]->enabled()) {
            return QBrush(Qt::black);
        } else {
            return QBrush(Qt::red);
        }

        break;
    }
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
    }
    return QVariant::Invalid;
}

void CollectedPeaksModel::sort(int column, Qt::SortOrder order)
{
    std::function<bool(nsx::sptrPeak3D, nsx::sptrPeak3D)> compareFn = [](nsx::sptrPeak3D, nsx::sptrPeak3D) { return false; };

    switch (column) {
    case Column::h: {
        compareFn = [&](nsx::sptrPeak3D p1, nsx::sptrPeak3D p2) {
            auto cell1 = p1->unitCell();
            auto cell2 = p2->unitCell();
            if (cell1 && cell2){
                nsx::MillerIndex miller_index1(p1->q(), *cell1);
                nsx::MillerIndex miller_index2(p2->q(), *cell2);
                return (miller_index1[0]<miller_index2[0]);
            } else {
                return ((cell1 != nullptr) < (cell2 != nullptr));
            }
        };
        break;
    }
    case Column::k: {
        compareFn = [&](nsx::sptrPeak3D p1, nsx::sptrPeak3D p2) {
            auto cell1 = p1->unitCell();
            auto cell2 = p2->unitCell();
            if (cell1 && cell2){
                nsx::MillerIndex miller_index1(p1->q(), *cell1);
                nsx::MillerIndex miller_index2(p2->q(), *cell2);
                return (miller_index1[1]<miller_index2[1]);
            } else {
                return ((cell1 != nullptr) < (cell2 != nullptr));
            }
        };
        break;
    }
    case Column::l: {
        compareFn = [](nsx::sptrPeak3D p1, nsx::sptrPeak3D p2) {
            auto cell1 = p1->unitCell();
            auto cell2 = p2->unitCell();
            if (cell1 && cell2){
                nsx::MillerIndex miller_index1(p1->q(), *cell1);
                nsx::MillerIndex miller_index2(p2->q(), *cell2);
                return (miller_index1[2]<miller_index2[2]);
            } else {
                return ((cell1 != nullptr) < (cell2 != nullptr));
            }
        };
        break;
    }
    case Column::px: {
        compareFn = [](nsx::sptrPeak3D p1, nsx::sptrPeak3D p2) {
            auto center1 = p1->shape().center();
            auto center2 = p2->shape().center();
            return (center1[0] < center2[0]);
        };
        break;
    }
    case Column::py: {
        compareFn = [](nsx::sptrPeak3D p1, nsx::sptrPeak3D p2) {
            auto center1 = p1->shape().center();
            auto center2 = p2->shape().center();
            return (center1[1] < center2[1]);
        };
        break;
    }
    case Column::frame: {
        compareFn = [](nsx::sptrPeak3D p1, nsx::sptrPeak3D p2) {
            auto center1 = p1->shape().center();
            auto center2 = p2->shape().center();
            return (center1[2] < center2[2]);
        };
        break;
    }
    case  Column::intensity: {
        compareFn = [](nsx::sptrPeak3D p1, nsx::sptrPeak3D p2) {
            auto intensity1 = p1->correctedIntensity().value();
            auto intensity2 = p2->correctedIntensity().value();
            return (intensity1 < intensity2);
        };
        break;
    }
    case Column::sigmaIntensity: {
        compareFn = [](nsx::sptrPeak3D p1, nsx::sptrPeak3D p2) {
            auto sigma_intensity1 = p1->correctedIntensity().sigma();
            auto sigma_intensity2 = p2->correctedIntensity().sigma();
            return (sigma_intensity1 < sigma_intensity2);
        };
        break;
    }
    case Column::numor: {
        compareFn = [&](nsx::sptrPeak3D p1, nsx::sptrPeak3D p2) {
            int numor1=p1->data()->metadata()->key<int>("Numor");
            int numor2=p2->data()->metadata()->key<int>("Numor");
            return (numor1 < numor2);
        };
        break;
    }
    case Column::unitCell: {
        compareFn = [&](nsx::sptrPeak3D p1, const nsx::sptrPeak3D p2) {
            auto uc1 = p1->unitCell();
            auto uc2 = p2->unitCell();
            std::string uc1Name = uc1 ? uc1->name() : "";
            std::string uc2Name = uc2 ? uc2->name() : "";
            return (uc2Name<uc1Name);
        };
        break;
    }
    }
    std::sort(_peaks.begin(), _peaks.end(), compareFn);

    if (order == Qt::DescendingOrder) {
        std::reverse(_peaks.begin(),_peaks.end());
    }
    emit layoutChanged();
}

void CollectedPeaksModel::togglePeakSelection(QModelIndex peak_index)
{
    int row = peak_index.row();

    auto peak = _peaks[row];

    peak->setSelected(!(peak->selected()));

    QModelIndex topleft_index = index(row,0);
    QModelIndex bottomright_index = index(row,columnCount(QModelIndex())-1);

    emit dataChanged(topleft_index,bottomright_index);

    emit _session->signalEnabledPeakChanged(peak);
}

void CollectedPeaksModel::selectPeak(const QModelIndex& index)
{
    auto selected_peak = _peaks[index.row()];

    emit signalSelectedPeakChanged(selected_peak);
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
        for (int i=0;i<rowCount(QModelIndex());++i) {
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

    QModelIndex topleft_index = index(0,0);
    QModelIndex bottomright_index = index(rowCount(QModelIndex())-1,columnCount(QModelIndex())-1);

    emit dataChanged(topleft_index,bottomright_index);
}

QModelIndexList CollectedPeaksModel::unindexedPeaks()
{
    QModelIndexList list;

    for (int i=0; i<rowCount(QModelIndex()); ++i) {
        auto peak = _peaks[i];
        if (!peak->unitCell()) {
            list.append(index(i,0));
        }
    }
    return list;
}

QModelIndexList CollectedPeaksModel::selectedPeaks()
{
    QModelIndexList list;

    for (int i=0; i<rowCount(QModelIndex()); ++i) {
        auto peak = _peaks[i];
        if (peak->enabled()) {
            list.append(index(i,0));
        }
    }
    return list;
}

nsx::sptrExperiment CollectedPeaksModel::experiment()
{
    return _experiment;
}
