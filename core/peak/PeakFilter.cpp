//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/peak/PeakFilter.cpp
//! @brief     Implements ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include <algorithm>
#include <iterator>
#include <limits>
#include <map>
#include <set>

#include "core/experiment/DataSet.h"
#include "core/instrument/Diffractometer.h"
#include "core/peak/MergedData.h"
#include "core/crystal/MillerIndex.h"
#include "core/peak/Octree.h"
#include "core/peak/Peak3D.h"
#include "core/peak/PeakFilter.h"
#include "core/rec_space/ReciprocalVector.h"
#include "core/instrument/Sample.h"
#include "core/crystal/SpaceGroup.h"
#include "core/crystal/UnitCell.h"

namespace nsx {

PeakList PeakFilter::mergedPeaksSignificance(const PeakList& peaks, double significance_level) const
{
    PeakFilter peak_filter;

    PeakList filtered_peaks = peak_filter.hasUnitCell(peaks);

    std::map<nsx::sptrUnitCell, PeakList> peaks_per_unit_cell;

    for (auto peak : filtered_peaks) {

        auto unit_cell = peak->unitCell();

        auto it = peaks_per_unit_cell.find(unit_cell);

        if (it == peaks_per_unit_cell.end()) {
            peaks_per_unit_cell.insert(std::make_pair(unit_cell, PeakList({peak})));
        } else {
            it->second.push_back(peak);
        }
    }

    PeakList bad_peaks;

    for (auto p : peaks_per_unit_cell) {

        auto unit_cell = p.first;

        SpaceGroup group(unit_cell->spaceGroup());

        MergedData merged(group, true);

        PeakList filtered_peaks;
        filtered_peaks =
            peak_filter.indexed(filtered_peaks, *unit_cell, unit_cell->indexingTolerance());

        for (auto peak : filtered_peaks) {
            merged.addPeak(peak);
        }

        for (auto&& merged_peak : merged.peaks()) {
            // p value too high: reject peaks
            if (merged_peak.pValue() > significance_level) {
                for (auto&& p : merged_peak.peaks()) {
                    bad_peaks.push_back(p);
                }
            }
        }
    }

    for (auto it = filtered_peaks.begin(); it != filtered_peaks.end();) {
        auto jt = std::find(bad_peaks.begin(), bad_peaks.end(), *it);
        if (jt != bad_peaks.end()) {
            it = filtered_peaks.erase(it);
            bad_peaks.erase(jt);
        } else {
            ++it;
        }
    }

    return filtered_peaks;
}

PeakList PeakFilter::sparseDataSet(const PeakList& peaks, size_t min_num_peaks)
{

    PeakList filtered_peaks;

    // Gather the peaks per dataset
    std::map<sptrDataSet, PeakList> peaks_per_dataset;
    for (auto peak : peaks) {
        auto data = peak->data();
        if (!data) {
            continue;
        }

        auto it = peaks_per_dataset.find(data);
        if (it == peaks_per_dataset.end()) {
            peaks_per_dataset.insert(std::make_pair(data, PeakList({peak})));
        } else {
            it->second.push_back(peak);
        }
    }

    // Keep the peaks that belong to datasets with a number of peaks >
    // min_num_peaks
    for (auto p : peaks_per_dataset) {
        if (p.second.size() < min_num_peaks) {
            continue;
        }

        for (auto peak : p.second) {
            filtered_peaks.push_back(peak);
        }
    }

    return filtered_peaks;
}

PeakList PeakFilter::extincted(const PeakList& peaks)
{
    PeakFilter peak_filter;

    PeakList filtered_peaks = peak_filter.hasUnitCell(peaks);

    std::map<nsx::sptrUnitCell, PeakList> peaks_per_unit_cell;

    for (auto peak : filtered_peaks) {

        auto unit_cell = peak->unitCell();

        auto it = peaks_per_unit_cell.find(unit_cell);

        if (it == peaks_per_unit_cell.end()) {
            peaks_per_unit_cell.insert(std::make_pair(unit_cell, PeakList({peak})));
        } else {
            it->second.push_back(peak);
        }
    }

    PeakList extincted_peaks;

    for (auto p : peaks_per_unit_cell) {

        PeakList indexed_peaks =
            peak_filter.indexed(p.second, *(p.first), p.first->indexingTolerance());

        SpaceGroup group(p.first->spaceGroup());

        MergedData merged(group, true);

        for (auto peak : indexed_peaks) {

            merged.addPeak(peak);

            MillerIndex hkl(peak->q(), *(p.first));

            if (group.isExtinct(hkl)) {
                extincted_peaks.push_back(peak);
            }
        }
    }

    for (auto it = filtered_peaks.begin(); it != filtered_peaks.end();) {
        auto jt = std::find(extincted_peaks.begin(), extincted_peaks.end(), *it);
        if (jt != extincted_peaks.end()) {
            it = filtered_peaks.erase(it);
            extincted_peaks.erase(jt);
        } else {
            ++it;
        }
    }

    return filtered_peaks;
}

PeakList PeakFilter::overlapping(const PeakList& peaks)
{
    PeakList filtered_peaks(peaks);

    std::vector<Ellipsoid> ellipsoids;
    std::set<Octree::collision_pair> collisions;

    Eigen::Vector3d lower(
        std::numeric_limits<double>::infinity(), std::numeric_limits<double>::infinity(),
        std::numeric_limits<double>::infinity());
    Eigen::Vector3d upper(-lower);

    for (auto peak : peaks) {

        auto&& ellipsoid = peak->shape();
        ellipsoids.emplace_back(ellipsoid);

        Eigen::Vector3d p = ellipsoid.center();

        for (int i = 0; i < 3; ++i) {
            lower(i) = std::min(lower(i), p(i));
            upper(i) = std::max(upper(i), p(i));
        }
    }

    // build octree
    Octree tree(lower, upper);

    for (unsigned int i = 0; i < peaks.size(); ++i) {
        tree.addData(&ellipsoids[i]);
    }

    collisions = tree.getCollisions();

    PeakList colliding_peaks;

    // handle collisions below
    for (auto collision : collisions) {
        unsigned int i = collision.first - &ellipsoids[0];
        unsigned int j = collision.second - &ellipsoids[0];
        colliding_peaks.push_back(peaks[i]);
        colliding_peaks.push_back(peaks[j]);
    }

    for (auto it = filtered_peaks.begin(); it != filtered_peaks.end();) {
        auto jt = std::find(colliding_peaks.begin(), colliding_peaks.end(), *it);
        if (jt != colliding_peaks.end()) {
            it = filtered_peaks.erase(it);
            colliding_peaks.erase(jt);
        } else {
            ++it;
        }
    }

    return filtered_peaks;
}

PeakList PeakFilter::complementary(const PeakList& peaks, const PeakList& other_peaks) const
{
    PeakList filtered_peaks;

    for (auto peak : peaks) {
        auto it = std::find(other_peaks.begin(), other_peaks.end(), peak);
        if (it == other_peaks.end()) {
            filtered_peaks.push_back(peak);
        }
    }

    return filtered_peaks;
}

PeakList PeakFilter::enabled(const PeakList& peaks, bool flag) const
{
    PeakList filtered_peaks;

    std::copy_if(
        peaks.begin(), peaks.end(), std::back_inserter(filtered_peaks),
        [flag](sptrPeak3D peak) { return flag == peak->enabled(); });

    return filtered_peaks;
}

PeakList PeakFilter::selected(const PeakList& peaks, bool flag) const
{
    PeakList filtered_peaks;

    std::copy_if(
        peaks.begin(), peaks.end(), std::back_inserter(filtered_peaks),
        [flag](sptrPeak3D peak) { return flag == peak->selected(); });

    return filtered_peaks;
}

PeakList PeakFilter::masked(const PeakList& peaks, bool flag) const
{
    PeakList filtered_peaks;

    std::copy_if(
        peaks.begin(), peaks.end(), std::back_inserter(filtered_peaks),
        [flag](sptrPeak3D peak) { return flag == peak->masked(); });

    return filtered_peaks;
}

PeakList PeakFilter::indexed(const PeakList& peaks, const UnitCell& cell, double tolerance) const
{
    PeakList filtered_peaks;

    for (auto peak : peaks) {

        MillerIndex miller_index(peak->q(), cell);
        if (miller_index.indexed(tolerance)) {
            filtered_peaks.push_back(peak);
        }
    }

    return filtered_peaks;
}

PeakList PeakFilter::indexed(const PeakList& peaks) const
{
    PeakList filtered_peaks;

    for (auto peak : peaks) {

        auto cell = peak->unitCell();

        if (!cell) {
            continue;
        }
        MillerIndex miller_index(peak->q(), *cell);
        if (miller_index.indexed(cell->indexingTolerance())) {
            filtered_peaks.push_back(peak);
        }
    }

    return filtered_peaks;
}

PeakList PeakFilter::unitCell(const PeakList& peaks, sptrUnitCell unit_cell) const
{
    PeakList filtered_peaks;

    for (auto peak : peaks) {
        if (peak->unitCell() == unit_cell) {
            filtered_peaks.push_back(peak);
        }
    }

    return filtered_peaks;
}

PeakList PeakFilter::strength(const PeakList& peaks, double min, double max) const
{
    PeakList filtered_peaks;

    for (auto peak : peaks) {

        auto corrected_intensity = peak->correctedIntensity();

        double intensity = corrected_intensity.value();
        double sigma = corrected_intensity.sigma();

        if (sigma < 1.0e-6) {
            continue;
        }

        double i_over_sigma = intensity / sigma;

        if (i_over_sigma >= min && i_over_sigma <= max) {
            filtered_peaks.push_back(peak);
        }
    }

    return filtered_peaks;
}

PeakList PeakFilter::predicted(const PeakList& peaks, bool flag) const
{
    PeakList filtered_peaks;

    std::copy_if(
        peaks.begin(), peaks.end(), std::back_inserter(filtered_peaks),
        [flag](sptrPeak3D peak) { return flag == peak->predicted(); });

    return filtered_peaks;
}

PeakList PeakFilter::dRange(const PeakList& peaks, double dmin, double dmax) const
{
    PeakList filtered_peaks;

    for (auto peak : peaks) {

        auto q = peak->q();

        double d = 1.0 / q.rowVector().norm();

        if (d >= dmin && d <= dmax) {
            filtered_peaks.push_back(peak);
        }
    }

    return filtered_peaks;
}

PeakList PeakFilter::hasUnitCell(const PeakList& peaks) const
{
    PeakList filtered_peaks;

    for (auto peak : peaks) {
        auto cell = peak->unitCell();
        if (cell != nullptr) {
            filtered_peaks.push_back(peak);
        }
    }

    return filtered_peaks;
}

} // end namespace nsx
