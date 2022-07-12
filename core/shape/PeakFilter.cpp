//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/shape/PeakFilter.cpp
//! @brief     Implements class PeakFilter
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

#include "base/geometry/ReciprocalVector.h"
#include "base/utils/Logger.h"
#include "core/data/DataSet.h"
#include "core/instrument/Diffractometer.h"
#include "core/instrument/Sample.h"
#include "core/peak/Peak3D.h"
#include "core/shape/Octree.h"
#include "core/shape/PeakFilter.h"
#include "core/statistics/MergedData.h"
#include "tables/crystal/MillerIndex.h"
#include "tables/crystal/SpaceGroup.h"
#include "tables/crystal/UnitCell.h"

namespace nsx {

PeakFilter::PeakFilter()
{
    _filter_params = std::make_unique<PeakFilterParameters>();
    _filter_flags = std::make_unique<PeakFilterFlags>();
    resetFilterFlags();
}

void PeakFilter::resetFilterFlags()
{
    *_filter_flags = {true,  false, false, false, false, false, false, false,
                      false, false, false, false, false, false, false};
}

void PeakFilter::filterSignificance(PeakCollection* peak_collection) const
{
    filterHasUnitCell(peak_collection);
    filterIndexTolerance(peak_collection);
    std::map<const nsx::UnitCell*, std::vector<nsx::Peak3D*>> peaks_per_unit_cell;
    for (int i = 0; i < peak_collection->numberOfPeaks(); ++i) {
        nsx::Peak3D* peak_ptr = peak_collection->getPeak(i);
        if (peak_ptr->caughtByFilter()) {
            const UnitCell* unit_cell = peak_ptr->unitCell();
            auto it = peaks_per_unit_cell.find(unit_cell);
            if (it == peaks_per_unit_cell.end()) {
                std::vector<nsx::Peak3D*> temp{peak_ptr};
                peaks_per_unit_cell.insert(std::make_pair(unit_cell, temp));
            } else {
                it->second.push_back(peak_ptr);
            }
        }
    }

    for (const auto& [cell, peaks] : peaks_per_unit_cell) {
        (void)peaks;
        std::vector<PeakCollection*> collection_vector;
        collection_vector.push_back(peak_collection);
        const std::vector<Peak3D*> filtered_peaks = peak_collection->getFilteredPeakList();

        MergedData merged(cell->spaceGroup(), collection_vector, true);
        for (const auto& peak : filtered_peaks)
            merged.addPeak(peak);

        for (const auto& merged_peak : merged.mergedPeakSet()) {
            if (merged_peak.pValue() > _filter_params->significance) {
                for (const auto& m : merged_peak.peaks())
                    m->rejectYou(true);
            }
        }
    }
}

void PeakFilter::filterSparseDataSet(PeakCollection* peak_collection) const
{
    std::map<sptrDataSet, std::vector<nsx::Peak3D*>> peaks_per_dataset;

    for (int i = 0; i < peak_collection->numberOfPeaks(); ++i) {
        nsx::Peak3D* peak_ptr = peak_collection->getPeak(i);
        const auto& data = peak_ptr->dataSet();
        if (!data)
            continue;

        auto it = peaks_per_dataset.find(data);
        if (it == peaks_per_dataset.end()) {
            std::vector<nsx::Peak3D*> temp{peak_ptr};
            peaks_per_dataset.insert(std::make_pair(data, temp));
        } else {
            it->second.push_back(peak_ptr);
        }
    }

    int nrejected = 0;
    for (const auto& p : peaks_per_dataset) {
        if (p.second.size() > _filter_params->sparse) {
            for (auto peak : p.second)
                peak->caughtYou(true);
        } else {
            for (auto peak : p.second) {
                peak->rejectYou(true);
                ++nrejected;
            }
        }
    }
    nsxlog(Level::Info, "PeakFilter::filterSparseDataSet: ", nrejected, " peaks rejected");
}

void PeakFilter::filterExtinct(PeakCollection* peak_collection) const
{
    filterHasUnitCell(peak_collection);
    filterIndexTolerance(peak_collection);
    int nrejected = 0;
    for (int i = 0; i < peak_collection->numberOfPeaks(); ++i) {
        nsx::Peak3D* peak_ptr = peak_collection->getPeak(i);
        if (peak_ptr->caughtByFilter()) {
            const SpaceGroup group(peak_ptr->unitCell()->spaceGroup());
            if (group.isExtinct(peak_ptr->hkl())) {
                peak_ptr->rejectYou(true);
                ++nrejected;
            }
        }
    }
    nsxlog(Level::Info, "PeakFilter::filterExtinct: ", nrejected, " peaks rejected");
}

void PeakFilter::filterOverlapping(PeakCollection* peak_collection) const
{
    std::vector<Ellipsoid> ellipsoids;
    Eigen::Vector3d lower(
        std::numeric_limits<double>::infinity(), std::numeric_limits<double>::infinity(),
        std::numeric_limits<double>::infinity());
    Eigen::Vector3d upper(-lower);

    for (int i = 0; i < peak_collection->numberOfPeaks(); ++i) {
        nsx::Peak3D* peak_ptr = peak_collection->getPeak(i);
        peak_ptr->caughtYou(true);
        auto&& ellipsoid = peak_ptr->shape();
        ellipsoids.emplace_back(ellipsoid);

        Eigen::Vector3d p = ellipsoid.center();

        for (int i = 0; i < 3; ++i) {
            lower(i) = std::min(lower(i), p(i));
            upper(i) = std::max(upper(i), p(i));
        }
    }

    // build octree
    Octree tree(lower, upper);
    for (unsigned int i = 0; i < peak_collection->numberOfPeaks(); ++i)
        tree.addData(&ellipsoids[i]);

    // handle collisions below
    int nrejected = 0;
    for (auto collision : tree.getCollisions(_filter_params->peak_end, _filter_params->bkg_end)) {
        unsigned int i = collision.first - &ellipsoids[0];
        unsigned int j = collision.second - &ellipsoids[0];
        peak_collection->getPeak(i)->rejectYou(true);
        peak_collection->getPeak(j)->rejectYou(true);
        nrejected += 2;
    }
    nsxlog(Level::Info, "PeakFilter::filterOverlapping: ", nrejected, " peaks rejected");
}

void PeakFilter::filterComplementary(PeakCollection* /*peak_collection*/) const
{
    // PeakList filtered_peaks;

    // for (auto peak : peaks) {
    //     auto it = std::find(other_peaks.begin(), other_peaks.end(), peak);
    //     if (it == other_peaks.end())
    //         filtered_peaks.push_back(peak);
    // }
}

void PeakFilter::filterEnabled(PeakCollection* peak_collection) const
{
    int nrejected = 0;
    for (int i = 0; i < peak_collection->numberOfPeaks(); ++i) {
        nsx::Peak3D* peak_ptr = peak_collection->getPeak(i);
        if (peak_ptr->enabled())
            peak_ptr->caughtYou(true);
        else {
            peak_ptr->rejectYou(true);
            ++nrejected;
        }
    }
    nsxlog(Level::Info, "PeakFilter::filterEnabled: ", nrejected, " peaks rejected");
}

std::vector<Peak3D*> PeakFilter::filterEnabled(
    const std::vector<Peak3D*> input_peaks, bool flag) const
{
    std::vector<Peak3D*> filtered_peaks;
    std::copy_if(
        input_peaks.begin(), input_peaks.end(), std::back_inserter(filtered_peaks),
        [flag](Peak3D* peak) { return flag == peak->enabled(); });
    return filtered_peaks;
}

void PeakFilter::filterSelected(PeakCollection* peak_collection) const
{
    int nrejected = 0;
    for (int i = 0; i < peak_collection->numberOfPeaks(); ++i) {
        nsx::Peak3D* peak_ptr = peak_collection->getPeak(i);
        if (peak_ptr->selected())
            peak_ptr->caughtYou(true);
        else {
            peak_ptr->rejectYou(true);
            ++nrejected;
        }
    }
    nsxlog(Level::Info, "PeakFilter::filterSelected: ", nrejected, " peaks rejected");
}

void PeakFilter::filterMasked(PeakCollection* peak_collection) const
{
    int nrejected = 0;
    for (int i = 0; i < peak_collection->numberOfPeaks(); ++i) {
        nsx::Peak3D* peak_ptr = peak_collection->getPeak(i);
        if (peak_ptr->masked())
            peak_ptr->caughtYou(true);
        else {
            peak_ptr->rejectYou(true);
            ++nrejected;
        }
    }
    nsxlog(Level::Info, "PeakFilter::filterMasked: ", nrejected, " peaks rejected");
}

void PeakFilter::filterIndexed(PeakCollection* peak_collection) const
{
    filterHasUnitCell(peak_collection);
    int nrejected = 0;
    for (int i = 0; i < peak_collection->numberOfPeaks(); ++i) {
        nsx::Peak3D* peak_ptr = peak_collection->getPeak(i);
        if (peak_ptr->caughtByFilter()) {
            if (peak_ptr->hkl().indexed(_filter_params->unit_cell_tolerance))
                peak_ptr->caughtYou(true);
            else {
                peak_ptr->rejectYou(true);
                ++nrejected;
            }
        }
    }
    nsxlog(Level::Info, "PeakFilter::filterIndexed: ", nrejected, " peaks rejected");
}

std::vector<Peak3D*> PeakFilter::filterIndexed(
    const std::vector<Peak3D*> peaks, const UnitCell* cell /* = nullptr  */) const
{
    // reset filters
    for (auto* peak : peaks) {
        peak->caughtYou(false);
        peak->rejectYou(false);
    }

    // If a cell is passed, use it to compute the Miller index, otherwise use the cell
    // specified by the peak
    std::vector<Peak3D*> filtered_peaks;
    for (auto peak : peaks) {
        MillerIndex index;
        double tol = 0;
        if (cell) {
            index = MillerIndex(peak->q(), *cell);
            tol = cell->indexingTolerance();
        } else {
            const UnitCell* batch_cell = peak->unitCell();
            if (!batch_cell)
                continue;
            index = MillerIndex(peak->q(), *batch_cell);
            tol = batch_cell->indexingTolerance();
        }

        if (index.indexed(tol)) {
            filtered_peaks.push_back(peak);
            peak->caughtYou(true);
        } else {
            peak->rejectYou(true);
        }
    }
    return filtered_peaks;
}

void PeakFilter::filterIndexTolerance(PeakCollection* peak_collection) const
{
    int nrejected = 0;
    for (int i = 0; i < peak_collection->numberOfPeaks(); ++i) {
        nsx::Peak3D* peak_ptr = peak_collection->getPeak(i);
        auto cell = peak_ptr->unitCell();
        if (!cell)
            continue;
        if (peak_ptr->hkl().indexed(cell->indexingTolerance()))
            peak_ptr->caughtYou(true);
        else {
            peak_ptr->rejectYou(true);
            ++nrejected;
        }
    }
    nsxlog(Level::Info, "PeakFilter::filterIndexTolerance: ", nrejected, " peaks rejected");
}

void PeakFilter::filterUnitCell(PeakCollection* peak_collection) const
{
    int nrejected = 0;
    for (int i = 0; i < peak_collection->numberOfPeaks(); ++i) {
        nsx::Peak3D* peak_ptr = peak_collection->getPeak(i);
        if (peak_ptr->unitCell()->name() == _filter_params->unit_cell)
            peak_ptr->caughtYou(true);
        else {
            peak_ptr->rejectYou(true);
            ++nrejected;
        }
    }
    nsxlog(Level::Info, "PeakFilter::filterUnitCell: ", nrejected, " peaks rejected");
}

void PeakFilter::filterStrength(PeakCollection* peak_collection) const
{
    // Reject peaks with: i) zero sigma ii) strength (I/sigma) outside range iii) intensity NaN
    int nrejected = 0;
    for (int i = 0; i < peak_collection->numberOfPeaks(); ++i) {
        nsx::Peak3D* peak_ptr = peak_collection->getPeak(i);
        Intensity corrected_intensity = peak_ptr->correctedIntensity();
        if (!corrected_intensity.isValid()) {
            peak_ptr->rejectYou(true);
            ++nrejected;
            continue;
        }

        double intensity = corrected_intensity.value();
        double sigma = corrected_intensity.sigma();

        if (sigma < 1.0e-6) {
            peak_ptr->rejectYou(true);
            ++nrejected;
            continue;
        }

        double i_over_sigma = intensity / sigma;
        if (i_over_sigma >= _filter_params->strength_min
            && i_over_sigma <= _filter_params->strength_max)
            peak_ptr->caughtYou(true);
        else {
            peak_ptr->rejectYou(true);
            ++nrejected;
        }
    }
    nsxlog(Level::Info, "PeakFilter::filterStrength: ", nrejected, " peaks rejected");
}

void PeakFilter::filterPredicted(PeakCollection* peak_collection) const
{
    int nrejected = 0;
    for (int i = 0; i < peak_collection->numberOfPeaks(); ++i) {
        nsx::Peak3D* peak_ptr = peak_collection->getPeak(i);
        if (peak_ptr->predicted())
            peak_ptr->caughtYou(true);
        else {
            peak_ptr->rejectYou(true);
            ++nrejected;
        }
    }
    nsxlog(Level::Info, "PeakFilter::filterPredicted: ", nrejected, " peaks rejected");
}

void PeakFilter::filterDRange(PeakCollection* peak_collection) const
{
    int nrejected = 0;
    for (int i = 0; i < peak_collection->numberOfPeaks(); ++i) {
        auto* peak_ptr = peak_collection->getPeak(i);
        auto q = peak_ptr->q();
        if (!q.isValid()) {
            peak_ptr->rejectYou(true);
            ++nrejected;
            continue;
        }

        double d = 1.0 / q.rowVector().norm();
        if ((d >= _filter_params->d_min && d <= _filter_params->d_max))
            peak_ptr->caughtYou(true);
        else {
            peak_ptr->rejectYou(true);
            ++nrejected;
        }
    }
    nsxlog(Level::Info, "PeakFilter::filterDRange: ", nrejected, " peaks rejected");
}

void PeakFilter::filterHasUnitCell(PeakCollection* peak_collection) const
{
    int nrejected = 0;
    nsxlog(Level::Info, "Filtering out peaks without unit cell");
    for (int i = 0; i < peak_collection->numberOfPeaks(); ++i) {
        nsx::Peak3D* peak_ptr = peak_collection->getPeak(i);
        auto cell = peak_ptr->unitCell();
        if (cell != nullptr)
            peak_ptr->caughtYou(true);
        else {
            peak_ptr->rejectYou(true);
            ++nrejected;
        }
    }
    nsxlog(Level::Info, "PeakFilter::filterHasUnitCell: ", nrejected, " peaks rejected");
}

void PeakFilter::filterFrameRange(PeakCollection* peak_collection) const
{
    int nrejected = 0;
    for (int i = 0; i < peak_collection->numberOfPeaks(); ++i) {
        nsx::Peak3D* peak_ptr = peak_collection->getPeak(i);
        auto c = peak_ptr->shape().center();
        if (c[2] >= _filter_params->frame_min && c[2] <= _filter_params->frame_max)
            peak_ptr->caughtYou(true);
        else {
            peak_ptr->rejectYou(true);
            ++nrejected;
        }
    }
    nsxlog(Level::Info, "PeakFilter::filterFrameRange: ", nrejected, " peaks rejected");
}

void PeakFilter::filterRejectionFlag(PeakCollection* peak_collection) const
{
    int ncaught = 0;
    for (Peak3D* peak : peak_collection->getPeakList()) {
        if (peak->rejectionFlag() == _filter_params->rejection_flag) {
            peak->caughtYou(true);
            ++ncaught;
        } else
            peak->rejectYou(true);
    }
    nsxlog(Level::Info, "PeakFilter::filterRejectionFlag: ", ncaught, " peaks caught");
}

void PeakFilter::filter(PeakCollection* peak_collection) const
{
    nsxlog(Level::Info, "PeakFilter::filter: filtering peaks");
    if (_filter_flags->state) {
        if (_filter_flags->selected) {
            nsxlog(Level::Info, "Filtering out unselected peaks");
            filterSelected(peak_collection);
        }
        if (_filter_flags->masked) {
            nsxlog(Level::Info, "Filtering out unmasked peaks");
            filterMasked(peak_collection);
        }
        if (_filter_flags->predicted) {
            nsxlog(Level::Info, "Filtering by prediction");
            filterPredicted(peak_collection);
        }
    }

    if (_filter_flags->indexed) {
        nsxlog(Level::Info, "Filtering out unindexed peaks");
        filterIndexed(peak_collection);
    }

    if (_filter_flags->index_tol) {
        if (!_filter_params->unit_cell.empty()) {
            nsxlog(Level::Info, "Filtering by Miller index tolerance");
            filterIndexTolerance(peak_collection);
        }
    }

    if (_filter_flags->strength) {
        nsxlog(Level::Info, "Filtering by peak strength (I/sigma)");
        filterStrength(peak_collection);
    }

    if (_filter_flags->d_range) {
        nsxlog(Level::Info, "Filtering by D-range");
        filterDRange(peak_collection);
    }

    if (_filter_flags->extinct) {
        nsxlog(Level::Info, "Filtering out extinct peaks");
        filterExtinct(peak_collection);
    }

    if (_filter_flags->sparse) {
        nsxlog(Level::Info, "Filtering sparse data set");
        filterSparseDataSet(peak_collection);
    }

    if (_filter_flags->significance) {
        nsxlog(Level::Info, "Filtering by significance");
        filterSignificance(peak_collection);
    }

    if (_filter_flags->overlapping) {
        nsxlog(Level::Info, "Filtering out overlapping peaks");
        filterOverlapping(peak_collection);
    }

    if (_filter_flags->complementary) { // this does nothing
        filterComplementary(peak_collection);
        nsxlog(Level::Info, "Filtering complementary");
    }

    if (_filter_flags->frames) {
        filterFrameRange(peak_collection);
        nsxlog(
            Level::Info, "Filtering peaks from frames in range", _filter_params->frame_min, "-",
            _filter_params->frame_max);
    }

    if (_filter_flags->rejection_flag) {
        filterRejectionFlag(peak_collection);
        nsxlog(
            Level::Info,
            "Filtering by rejection flag: ", static_cast<int>(_filter_params->rejection_flag));
    }
}

void PeakFilter::resetFiltering(PeakCollection* peak_collection) const
{
    for (int i = 0; i < peak_collection->numberOfPeaks(); ++i) {
        peak_collection->getPeak(i)->caughtYou(false);
        peak_collection->getPeak(i)->rejectYou(false);
    }
}

PeakFilterParameters* PeakFilter::parameters()
{
    return _filter_params.get();
}

PeakFilterFlags* PeakFilter::flags()
{
    return _filter_flags.get();
}

} // namespace nsx
