//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/statistics/MergedPeakCollection.cpp
//! @brief     Implements class MergedPeakCollection
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "core/statistics/MergedPeakCollection.h"

#include "base/utils/Logger.h"
#include "core/data/DataSet.h"
#include "core/data/DataTypes.h"
#include "tables/crystal/MillerIndex.h"
#include "tables/crystal/UnitCell.h"

namespace ohkl {

MergedPeakCollection::MergedPeakCollection(
    SpaceGroup space_group, std::vector<PeakCollection*> peak_collections, bool friedel,
    bool sum_intensity, int fmin, int fmax)
    : _group(space_group)
    , _friedel(friedel)
    , _merged_peak_set()
    , _sum_intensity(sum_intensity)
    , _frame_min(fmin)
    , _frame_max(fmax)
{
    ohklLog(Level::Info, "MergedPeakCollection::MergedPeakCollection: merging peaks");
    _peak_collections = peak_collections;
    for (int i = 0; i < _peak_collections.size(); ++i) {
        if (_peak_collections[i] == nullptr)
            continue;
        ohklLog(
            Level::Info, "MergedPeakCollection::MergedPeakCollection: peak collection ",
            _peak_collections[i]->name());
        std::vector<Peak3D*> peaks = _peak_collections[i]->getPeakList();
        for (int j = 0; j < peaks.size(); ++j)
            addPeak(peaks[j]);
    }
    if (_nInvalid > 0) {
        ohklLog(
            Level::Info, "MergedPeakCollection::MergedPeakCollection: ", totalSize(),
            " observed peaks");
        ohklLog(
            Level::Info, "MergedPeakCollection::MergedPeakCollection: ", _nInvalid,
            " disabled peaks");
        ohklLog(
            Level::Info, "MergedPeakCollection::MergedPeakCollection: ", _nInequivalent,
            " inequivalent peaks");
        ohklLog(
            Level::Info, "MergedPeakCollection::MergedPeakCollection: ", nUnique(),
            " symmetry-unique peaks");
    }
}

MergedPeakCollection::MergedPeakCollection(
    SpaceGroup space_group, bool friedel, bool sum_intensity, int fmin, int fmax)
    : _group(space_group)
    , _friedel(friedel)
    , _merged_peak_set()
    , _sum_intensity(sum_intensity)
    , _frame_min(fmin)
    , _frame_max(fmax)
{
}

MergedPeakCollection::MergedPeakCollection(
    SpaceGroup space_group, std::vector<PeakCollection*> peak_collections, bool friedel)
    : _group(space_group), _friedel(friedel), _merged_peak_set()
{
    ohklLog(
        Level::Info,
        "MergedPeakCollection::MergedPeakCollection: merging peaks for max completeness");
    _peak_collections = peak_collections;
    for (int i = 0; i < _peak_collections.size(); ++i) {
        if (_peak_collections[i] == nullptr)
            continue;
        ohklLog(
            Level::Info, "MergedPeakCollection::MergedPeakCollection: peak collection ",
            _peak_collections[i]->name());
        std::vector<Peak3D*> peaks = _peak_collections[i]->getPeakList();
        for (int j = 0; j < peaks.size(); ++j)
            addAny(peaks[j]);
    }
}

void MergedPeakCollection::addPeak(Peak3D* peak)
{
    auto c = peak->shape().center();
    // Ignore the peaks outside the frame range (mainly to exclude peaks that can't be interpolated)
    if (_frame_min >= 0 && _frame_max >= 0) {
        if (c[2] >= _frame_max && c[2] <= _frame_min)
            return;
    }

    if (peak->hkl() == MillerIndex(0, 0, 0))
        return;

    MergedPeak new_peak(_group, _sum_intensity, _friedel);

    MergeFlag flag = new_peak.addPeak(peak);
    if (flag == MergeFlag::Invalid) {
        ++_nInvalid;
        return;
    }

    auto it = _merged_peak_set.find(new_peak);

    if (it != _merged_peak_set.end()) { // Found this peak in the set already
        MergedPeak merged(*it);
        MergeFlag flag = merged.addPeak(peak);
        if (flag == MergeFlag::Inequivalent)
            ++_nInequivalent;
        _merged_peak_set.erase(it);
        _merged_peak_set.emplace(std::move(merged));
        return;
    }
    _merged_peak_set.emplace(std::move(new_peak));
}

void MergedPeakCollection::addAny(Peak3D* peak)
{
    if (peak->hkl() == MillerIndex(0, 0, 0))
        return;

    MergedPeak new_peak(_group, _sum_intensity, _friedel);

    MergeFlag flag = new_peak.addAny(peak);

    auto it = _merged_peak_set.find(new_peak);

    if (it != _merged_peak_set.end()) { // Found this peak in the set already
        MergedPeak merged(*it);
        merged.addAny(peak);
        _merged_peak_set.erase(it);
        _merged_peak_set.emplace(std::move(merged));
        return;
    }
    _merged_peak_set.emplace(std::move(new_peak));
}

void MergedPeakCollection::addPeakCollection(PeakCollection* peaks)
{
    _peak_collections.push_back(peaks);
}

const MergedPeakSet& MergedPeakCollection::mergedPeakSet() const
{
    return _merged_peak_set;
}

size_t MergedPeakCollection::totalSize() const
{
    size_t total = 0;

    for (const auto& peak : _merged_peak_set)
        total += peak.redundancy();
    return total;
}

double MergedPeakCollection::redundancy() const
{
    return double(totalSize()) / double(_merged_peak_set.size());
}

void MergedPeakCollection::clear()
{
    _merged_peak_set.clear();
}

void MergedPeakCollection::setDRange(const double d_min, const double d_max)
{
    _d_min = d_min;
    _d_max = d_max;
    double lambda = _peak_collections[0]->data()->wavelength();
    sptrUnitCell cell = _peak_collections[0]->unitCell();
    _max_peaks = cell->maxPeaks(d_min, d_max, lambda);
    ohklLog(
        Level::Info, "MergedPeakCollection::setDRange: ", _max_peaks, " maximum possible peaks");
}

void MergedPeakCollection::setDRange(
    const double d_min, const double d_max, sptrDataSet data, sptrUnitCell cell)
{
    _d_min = d_min;
    _d_max = d_max;
    double lambda = data->wavelength();
    _max_peaks = cell->maxPeaks(d_min, d_max, lambda);
    ohklLog(
        Level::Info, "MergedPeakCollection::setDRange: ", _max_peaks, " maximum possible peaks");
}

double MergedPeakCollection::dMin() const
{
    return _d_min;
}

double MergedPeakCollection::dMax() const
{
    return _d_max;
}

} // namespace ohkl
