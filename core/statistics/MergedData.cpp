//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/statistics/MergedData.cpp
//! @brief     Implements class MergedData
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "core/statistics/MergedData.h"
#include "base/utils/Logger.h"

namespace ohkl {

MergedData::MergedData(
    SpaceGroup space_group, std::vector<PeakCollection*> peak_collections, bool friedel, int fmin,
    int fmax)
    : _group(space_group), _friedel(friedel), _merged_peak_set(), _frame_min(fmin), _frame_max(fmax)
{
    ohklLog(Level::Info, "MergedData::MergedData: merging peaks");
    _peak_collections = peak_collections;
    for (int i = 0; i < _peak_collections.size(); ++i) {
        if (_peak_collections[i] == nullptr)
            continue;
        ohklLog(
            Level::Info, "MergedData::MergedData: peak collection ", _peak_collections[i]->name());
        std::vector<Peak3D*> peaks = _peak_collections[i]->getPeakList();
        for (int j = 0; j < peaks.size(); ++j)
            addPeak(peaks[j]);
    }
    if (_nInvalid > 0) {
        ohklLog(Level::Info, "MergedData::MergedData: ", _max_peaks, " maximum possible peaks");
        ohklLog(Level::Info, "MergedData::MergedData: ", totalSize(), " merged peaks");
        ohklLog(Level::Info, "MergedData::MergedData: ", _nExtinct, " extinct peaks");
        ohklLog(Level::Info, "MergedData::MergedData: ", _nInvalid, " disabled peaks");
    }
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
    ++_max_peaks;

    if (!peak->enabled()) {
        ++_nInvalid;
        return false;
    }
    if (!peak->unitCell()) {
        ++_nInvalid;
        return false;
    }
    MergedPeak new_peak(_group, _friedel);

    MergeFlag success = new_peak.addPeak(peak);
    if (success == MergeFlag::InvalidQ) { // Interpolation error check
        ++_nInvalid;
        return false;
    } else if (success == MergeFlag::Extinct) {
        --_max_peaks;
        ohklLog(Level::Info, "Extinct: ", peak->toString());
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

} // namespace ohkl
