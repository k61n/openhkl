//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/statistics/MergedData.cpp
//! @brief     Implements class MergedData
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "core/statistics/MergedData.h"
#include "base/utils/Logger.h"

namespace nsx {

MergedData::MergedData(
    std::vector<PeakCollection*> peak_collections, bool friedel, int fmin, int fmax)
    : _friedel(friedel), _merged_peak_set(), _frame_min(fmin), _frame_max(fmax)
{
    _peak_collections = peak_collections;
    const UnitCell* unit_cell{nullptr};

    for (int i = 0; i < _peak_collections.size(); ++i) {
        std::vector<Peak3D*> peaks = _peak_collections[i]->getPeakList();
        for (int j = 0; j < peaks.size(); ++j) {
            if (peaks[j]->unitCell())
                unit_cell = peaks[j]->unitCell();
        }
    }

    if (!unit_cell)
        return;

    for (int i = 0; i < _peak_collections.size(); ++i) {
        std::vector<Peak3D*> peaks = _peak_collections[i]->getPeakList();
        for (int j = 0; j < peaks.size(); ++j) {
            if (!(peaks[j]->unitCell() == unit_cell))
                return;
        }
    }

    _group = unit_cell->spaceGroup();

    for (int i = 0; i < _peak_collections.size(); ++i) {
        std::vector<Peak3D*> peaks = _peak_collections[i]->getPeakList();
        for (int j = 0; j < peaks.size(); ++j)
            addPeak(peaks[j]);
    }
    if (_nInvalid > 0)
        nsxlog(Level::Info, "MergedData::MergedData: ", _nInvalid, " disabled peaks");
}

MergedData::MergedData(SpaceGroup space_group, bool friedel, int fmin, int fmax)
    : _friedel(friedel), _merged_peak_set(), _frame_min(fmin), _frame_max(fmax)
{
    _group = space_group;
}

bool MergedData::addPeak(Peak3D* peak)
{
    auto c = peak->shape().center();
    // Ignore the peaks outside the frame range (mainly to exclude peaks that can't be interpolated)
    if (_frame_min >= 0 && _frame_max >= 0) {
        if (c[2] >= _frame_max && c[2] <= _frame_min)
            return false;
    }

    if (!peak->enabled()) {
        ++_nInvalid;
        ++_nPeaks;
        return false;
    }
    if (!peak->unitCell()) {
        ++_nInvalid;
        ++_nPeaks;
        return false;
    }
    MergedPeak new_peak(_group, _friedel);

    bool success = new_peak.addPeak(peak);
    if (!success) { // Interpolation error check
        ++_nInvalid;
        ++_nPeaks;
        return false;
    }

    auto it = _merged_peak_set.find(new_peak);

    if (it != _merged_peak_set.end()) { // Found this peak in the set already
        MergedPeak merged(*it);
        merged.addPeak(peak);
        _merged_peak_set.erase(it);
        _merged_peak_set.emplace(std::move(merged));
        return false;
    }
    _merged_peak_set.emplace(std::move(new_peak));
    ++_nPeaks;

    return true;
}

const MergedPeakSet& MergedData::mergedPeakSet() const
{
    return _merged_peak_set;
}

size_t MergedData::totalSize() const
{
    size_t total = 0;

    for (const auto& peak : _merged_peak_set)
        total += peak.redundancy();
    return total;
}

double MergedData::redundancy() const
{
    return double(totalSize()) / double(_merged_peak_set.size());
}

void MergedData::clear()
{
    _merged_peak_set.clear();
}

double MergedData::completeness()
{
    if (totalSize() == 0)
        return 0.0;
    return double(_nPeaks - _nInvalid) / double(_nPeaks);
}

void MergedData::setDRange(const double d_min, const double d_max)
{
    _d_min = d_min;
    _d_max = d_max;
}

double MergedData::dMin() const
{
    return _d_min;
}

double MergedData::dMax() const
{
    return _d_max;
}

} // namespace nsx
