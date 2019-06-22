//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/analyse/MergedData.cpp
//! @brief     Implements class MergedData
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "core/analyse/MergedData.h"

namespace nsx {

MergedData::MergedData(const SpaceGroup& grp, bool friedel)
    : _group(grp), _friedel(friedel), _mergedPeakSet()
{
}

bool MergedData::addPeak(const sptrPeak3D& peak)
{
    MergedPeak new_peak(_group, _friedel);
    new_peak.addPeak(peak);
    auto it = _mergedPeakSet.find(new_peak);

    if (it != _mergedPeakSet.end()) {
        MergedPeak merged(*it);
        merged.addPeak(peak);
        _mergedPeakSet.erase(it);
        _mergedPeakSet.emplace(std::move(merged));
        return false;
    }
    _mergedPeakSet.emplace(std::move(new_peak));
    return true;
}

const MergedPeakSet& MergedData::mergedPeakSet() const
{
    return _mergedPeakSet;
}

size_t MergedData::totalSize() const
{
    size_t total = 0;

    for (const auto& peak : _mergedPeakSet)
        total += peak.redundancy();
    return total;
}

double MergedData::redundancy() const
{
    return double(totalSize()) / double(_mergedPeakSet.size());
}

void MergedData::clear()
{
    _mergedPeakSet.clear();
}

} // namespace nsx
