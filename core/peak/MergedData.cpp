//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/peak/MergedData.cpp
//! @brief     Implements ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "core/peak/MergedData.h"

namespace nsx {

MergedData::MergedData(const SpaceGroup& grp, bool friedel)
    : _group(grp), _friedel(friedel), _peaks()
{
}

bool MergedData::addPeak(const sptrPeak3D& peak)
{
    MergedPeak new_peak(_group, _friedel);
    new_peak.addPeak(peak);
    auto it = _peaks.find(new_peak);

    if (it != _peaks.end()) {
        MergedPeak merged(*it);
        merged.addPeak(peak);
        _peaks.erase(it);
        _peaks.emplace(std::move(merged));
        return false;
    }
    _peaks.emplace(std::move(new_peak));
    return true;
}

const MergedPeakSet& MergedData::peaks() const
{
    return _peaks;
}

size_t MergedData::totalSize() const
{
    size_t total = 0;

    for (const auto& peak : _peaks) {
        total += peak.redundancy();
    }
    return total;
}

double MergedData::redundancy() const
{
    return double(totalSize()) / double(_peaks.size());
}

void MergedData::clear()
{
    _peaks.clear();
}

} // end namespace nsx
