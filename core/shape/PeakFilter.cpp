//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/shape/PeakFilter.cpp
//! @brief     Implements class PeakFilter
//!
//! @homepage  https://openhkl.org
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
#include "core/instrument/InstrumentState.h"
#include "core/instrument/Sample.h"
#include "core/shape/Octree.h"
#include "core/shape/PeakCollection.h"
#include "core/shape/PeakFilter.h"
#include "core/statistics/MergedPeakCollection.h"
#include "tables/crystal/MillerIndex.h"
#include "tables/crystal/SpaceGroup.h"
#include "tables/crystal/UnitCell.h"

namespace ohkl {

PeakFilter::PeakFilter()
{
    _filter_params = std::make_unique<PeakFilterParameters>();
    _filter_flags = std::make_unique<PeakFilterFlags>();
    resetFilterFlags();
}

void PeakFilter::resetFilterFlags()
{
    *_filter_flags = {true,  false, false, false, false, false, false, false, false,
                      false, false, false, false, false, false, false, false};
}

void PeakFilter::filterSignificance(PeakCollection* peak_collection) const
{
    filterHasUnitCell(peak_collection);
    filterIndexTolerance(peak_collection);
    std::map<const ohkl::UnitCell*, std::vector<ohkl::Peak3D*>> peaks_per_unit_cell;
    for (int i = 0; i < peak_collection->numberOfPeaks(); ++i) {
        ohkl::Peak3D* peak_ptr = peak_collection->getPeak(i);
        if (peak_ptr->caughtByFilter()) {
            const UnitCell* unit_cell = peak_ptr->unitCell();
            auto it = peaks_per_unit_cell.find(unit_cell);
            if (it == peaks_per_unit_cell.end()) {
                std::vector<ohkl::Peak3D*> temp{peak_ptr};
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

        MergedPeakCollection merged(
            cell->spaceGroup(), collection_vector, true, _filter_params->sum_intensities);
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
    std::map<sptrDataSet, std::vector<ohkl::Peak3D*>> peaks_per_dataset;

    for (int i = 0; i < peak_collection->numberOfPeaks(); ++i) {
        ohkl::Peak3D* peak_ptr = peak_collection->getPeak(i);
        const auto& data = peak_ptr->dataSet();
        if (!data)
            continue;

        auto it = peaks_per_dataset.find(data);
        if (it == peaks_per_dataset.end()) {
            std::vector<ohkl::Peak3D*> temp{peak_ptr};
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
    ohklLog(Level::Info, "PeakFilter::filterSparseDataSet: ", nrejected, " peaks rejected");
}

void PeakFilter::filterExtinct(PeakCollection* peak_collection) const
{
    filterHasUnitCell(peak_collection);
    filterIndexTolerance(peak_collection);
    int nrejected = 0;
    for (int i = 0; i < peak_collection->numberOfPeaks(); ++i) {
        ohkl::Peak3D* peak_ptr = peak_collection->getPeak(i);
        if (peak_ptr->caughtByFilter()) {
            const SpaceGroup group(peak_ptr->unitCell()->spaceGroup());
            if (group.isExtinct(peak_ptr->hkl())) {
                peak_ptr->rejectYou(true);
                ++nrejected;
            }
        }
    }
    ohklLog(Level::Info, "PeakFilter::filterExtinct: ", nrejected, " peaks rejected");
}

void PeakFilter::filterOverlapping(PeakCollection* peak_collection) const
{
    std::vector<Ellipsoid> ellipsoids;
    Eigen::Vector3d lower(
        std::numeric_limits<double>::infinity(), std::numeric_limits<double>::infinity(),
        std::numeric_limits<double>::infinity());
    Eigen::Vector3d upper(-lower);

    for (int i = 0; i < peak_collection->numberOfPeaks(); ++i) {
        ohkl::Peak3D* peak_ptr = peak_collection->getPeak(i);
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
    ohklLog(Level::Info, "PeakFilter::filterOverlapping: ", nrejected, " peaks rejected");
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
        ohkl::Peak3D* peak_ptr = peak_collection->getPeak(i);
        if (peak_ptr->enabled())
            peak_ptr->caughtYou(true);
        else {
            peak_ptr->rejectYou(true);
            ++nrejected;
        }
    }
    ohklLog(Level::Info, "PeakFilter::filterEnabled: ", nrejected, " peaks rejected");
}

std::vector<Peak3D*> PeakFilter::filterEnabled(
    const std::vector<Peak3D*>& input_peaks, bool flag) const
{
    std::vector<Peak3D*> filtered_peaks;
    std::copy_if(
        input_peaks.begin(), input_peaks.end(), std::back_inserter(filtered_peaks),
        [flag](Peak3D* peak) { return flag == peak->enabled(); });
    return filtered_peaks;
}

void PeakFilter::filterMasked(PeakCollection* peak_collection) const
{
    int nrejected = 0;
    for (int i = 0; i < peak_collection->numberOfPeaks(); ++i) {
        ohkl::Peak3D* peak_ptr = peak_collection->getPeak(i);
        if (peak_ptr->isRejectedFor(RejectionFlag::Masked)) {
            peak_ptr->rejectYou(true);
            ++nrejected;
        } else {
            peak_ptr->caughtYou(true);
        }
    }
    ohklLog(Level::Info, "PeakFilter::filterMasked: ", nrejected, " peaks rejected");
}

void PeakFilter::filterIndexed(PeakCollection* peak_collection) const
{
    filterHasUnitCell(peak_collection);
    int nrejected = 0;
    for (int i = 0; i < peak_collection->numberOfPeaks(); ++i) {
        ohkl::Peak3D* peak_ptr = peak_collection->getPeak(i);
        if (peak_ptr->caughtByFilter()) {
            if (peak_ptr->hkl().indexed(_filter_params->unit_cell_tolerance))
                peak_ptr->caughtYou(true);
            else {
                peak_ptr->rejectYou(true);
                ++nrejected;
            }
        }
    }
    ohklLog(Level::Info, "PeakFilter::filterIndexed: ", nrejected, " peaks rejected");
}

std::vector<Peak3D*> PeakFilter::filterIndexed(
    const std::vector<Peak3D*>& peaks, const UnitCell* cell /* = nullptr  */,
    const InstrumentState* state /* = nullptr */) const
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
        ReciprocalVector q;
        if (state)
            q = peak->q(*state);
        else
            q = peak->q();

        if (cell) {
            index = MillerIndex(q, *cell);
            tol = cell->indexingTolerance();
        } else {
            const UnitCell* batch_cell = peak->unitCell();
            if (!batch_cell)
                continue;
            index = MillerIndex(q, *batch_cell);
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
        ohkl::Peak3D* peak_ptr = peak_collection->getPeak(i);
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
    ohklLog(Level::Info, "PeakFilter::filterIndexTolerance: ", nrejected, " peaks rejected");
}

void PeakFilter::filterUnitCell(PeakCollection* peak_collection) const
{
    int nrejected = 0;
    for (int i = 0; i < peak_collection->numberOfPeaks(); ++i) {
        ohkl::Peak3D* peak_ptr = peak_collection->getPeak(i);
        if (peak_ptr->unitCell()->name() == _filter_params->unit_cell)
            peak_ptr->caughtYou(true);
        else {
            peak_ptr->rejectYou(true);
            ++nrejected;
        }
    }
    ohklLog(Level::Info, "PeakFilter::filterUnitCell: ", nrejected, " peaks rejected");
}

void PeakFilter::filterStrength(PeakCollection* peak_collection) const
{
    // Reject peaks with: i) zero sigma ii) strength (I/sigma) outside range iii) intensity NaN
    int nrejected = 0;
    for (int i = 0; i < peak_collection->numberOfPeaks(); ++i) {
        ohkl::Peak3D* peak_ptr = peak_collection->getPeak(i);
        Intensity corrected_intensity = peakIntensity(peak_ptr);

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
    ohklLog(
        Level::Info, "PeakFilter::filterStrength: filtering in range ",
        _filter_params->strength_min, " - ", _filter_params->strength_max);
    ohklLog(Level::Info, "PeakFilter::filterStrength: ", nrejected, " peaks rejected");
}

std::vector<Peak3D*> PeakFilter::filterStrength(
    const std::vector<Peak3D*>& peaks, double str_min, double str_max)
{
    std::vector<Peak3D*> filtered_peaks;
    for (auto* peak : peaks) {
        Intensity corrected_intensity = peakIntensity(peak);
        if (!corrected_intensity.isValid())
            continue;
        double intensity = corrected_intensity.value();
        double sigma = corrected_intensity.sigma();

        if (sigma < 1.0e-6)
            continue;

        double i_over_sigma = intensity / sigma;
        if (i_over_sigma >= str_min && i_over_sigma <= str_max)
            filtered_peaks.push_back(peak);
    }
    return filtered_peaks;
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
    ohklLog(Level::Info, "PeakFilter::filterDRange: ", nrejected, " peaks rejected");
}

std::vector<Peak3D*> PeakFilter::filterDRange(
    const std::vector<Peak3D*>& peaks, double d_min, double d_max,
    const InstrumentState* state) const
{
    std::vector<Peak3D*> filtered_peaks;
    for (auto* peak : peaks) {
        // If we only have one frame, we don't want to interpolate the InstrumentState
        ReciprocalVector rvec;
        if (state)
            rvec = peak->q(*state);
        else
            rvec = peak->q();

        if (!rvec.isValid())
            continue;
        double d = 1.0 / rvec.rowVector().norm();
        if (d >= d_min && d <= d_max) {
            filtered_peaks.push_back(peak);
        }
    }
    return filtered_peaks;
}

void PeakFilter::filterHasUnitCell(PeakCollection* peak_collection) const
{
    int nrejected = 0;
    ohklLog(Level::Info, "Filtering out peaks without unit cell");
    for (int i = 0; i < peak_collection->numberOfPeaks(); ++i) {
        ohkl::Peak3D* peak_ptr = peak_collection->getPeak(i);
        auto cell = peak_ptr->unitCell();
        if (cell != nullptr)
            peak_ptr->caughtYou(true);
        else {
            peak_ptr->rejectYou(true);
            ++nrejected;
        }
    }
    ohklLog(Level::Info, "PeakFilter::filterHasUnitCell: ", nrejected, " peaks rejected");
}

void PeakFilter::filterFrameRange(PeakCollection* peak_collection) const
{
    int nrejected = 0;
    for (int i = 0; i < peak_collection->numberOfPeaks(); ++i) {
        ohkl::Peak3D* peak_ptr = peak_collection->getPeak(i);
        auto c = peak_ptr->shape().center();
        if (c[2] >= _filter_params->first_frame && c[2] <= _filter_params->last_frame)
            peak_ptr->caughtYou(true);
        else {
            peak_ptr->rejectYou(true);
            ++nrejected;
        }
    }
    ohklLog(Level::Info, "PeakFilter::filterFrameRange: ", nrejected, " peaks rejected");
}

std::vector<Peak3D*> PeakFilter::filterFrameRange(
    const std::vector<Peak3D*>& peaks, int first_frame, int last_frame) const
{
    std::vector<Peak3D*> filtered_peaks;
    for (auto* peak : peaks) {
        auto c = peak->shape().center();
        if (c[2] >= static_cast<double>(first_frame) && c[2] <= static_cast<double>(last_frame))
            filtered_peaks.push_back(peak);
    }
    return filtered_peaks;
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
    ohklLog(Level::Info, "PeakFilter::filterRejectionFlag: ", ncaught, " peaks caught");
}

void PeakFilter::filterIntensity(PeakCollection* peak_collection) const
{
    int nrejected = 0;
    for (auto* peak : peak_collection->getPeakList()) {
        Intensity corrected_intensity = peakIntensity(peak);
        if (!corrected_intensity.isValid()) {
            peak->rejectYou(true);
            ++nrejected;
            continue;
        }
        double intensity = corrected_intensity.value();
        if (intensity >= _filter_params->intensity_min
            && intensity <= _filter_params->intensity_max) {
            peak->caughtYou(true);
        } else {
            peak->rejectYou(true);
            ++nrejected;
        }
    }
    ohklLog(
        Level::Info, "PeakFilter::filterIntensity: filtering in range ",
        _filter_params->intensity_min, " - ", _filter_params->intensity_max);
    ohklLog(Level::Info, "PeakFilter::filterIntensity: ", nrejected, " peaks rejected");
}

void PeakFilter::filterSigma(PeakCollection* peak_collection) const
{
    int nrejected = 0;
    for (auto* peak : peak_collection->getPeakList()) {
        Intensity corrected_intensity = peakIntensity(peak);
        if (!corrected_intensity.isValid()) {
            peak->rejectYou(true);
            ++nrejected;
            continue;
        }
        double sigma = corrected_intensity.sigma();
        if (sigma >= _filter_params->sigma_min && sigma <= _filter_params->sigma_max) {
            peak->caughtYou(true);
        } else {
            peak->rejectYou(true);
            ++nrejected;
        }
    }
    ohklLog(
        Level::Info, "PeakFilter::filterSigma: filtering in range ", _filter_params->sigma_min,
        " - ", _filter_params->sigma_max);
    ohklLog(Level::Info, "PeakFilter::filterSigma: ", nrejected, " peaks rejected");
}

void PeakFilter::filterGradient(PeakCollection* peak_collection) const
{
    int nrejected = 0;
    for (auto* peak : peak_collection->getPeakList()) {
        Intensity gradient = peak->meanBkgGradient();
        if (!gradient.isValid()) {
            peak->rejectYou(true);
            ++nrejected;
            continue;
        }
        double intensity = gradient.value();
        if (intensity >= _filter_params->gradient_min
            && intensity <= _filter_params->gradient_max) {
            peak->caughtYou(true);
        } else {
            peak->rejectYou(true);
            ++nrejected;
        }
    }
    ohklLog(
        Level::Info, "PeakFilter::filterGradient: filtering in range ",
        _filter_params->gradient_min, " - ", _filter_params->gradient_max);
    ohklLog(Level::Info, "PeakFilter::filterGradient: ", nrejected, " peaks rejected");
}

void PeakFilter::filterGradientSigma(PeakCollection* peak_collection) const
{
    int nrejected = 0;
    for (auto* peak : peak_collection->getPeakList()) {
        Intensity gradient = peak->meanBkgGradient();
        if (!gradient.isValid()) {
            peak->rejectYou(true);
            ++nrejected;
            continue;
        }
        double sigma = gradient.sigma();
        if (sigma >= _filter_params->gradient_sigma_min
            && sigma <= _filter_params->gradient_sigma_max) {
            peak->caughtYou(true);
        } else {
            peak->rejectYou(true);
            ++nrejected;
        }
    }
    ohklLog(
        Level::Info, "PeakFilter::filterGradientSigma: filtering in range ",
        _filter_params->gradient_sigma_min, " - ", _filter_params->gradient_sigma_max);
    ohklLog(Level::Info, "PeakFilter::filterGradientSigma: ", nrejected, " peaks rejected");
}

void PeakFilter::filter(PeakCollection* peak_collection) const
{
    ohklLog(Level::Info, "PeakFilter::filter: filtering peaks");

    if (_filter_flags->enabled) {
        ohklLog(Level::Info, "Filtering out disabled peaks");
        filterMasked(peak_collection);
    }

    if (_filter_flags->masked) {
        ohklLog(Level::Info, "Filtering out unmasked peaks");
        filterMasked(peak_collection);
    }

    if (_filter_flags->indexed) {
        ohklLog(Level::Info, "Filtering out unindexed peaks");
        filterIndexed(peak_collection);
    }

    if (_filter_flags->index_tol) {
        if (!_filter_params->unit_cell.empty()) {
            ohklLog(Level::Info, "Filtering by Miller index tolerance");
            filterIndexTolerance(peak_collection);
        }
    }

    if (_filter_flags->strength) {
        ohklLog(Level::Info, "Filtering by peak strength (I/sigma)");
        filterStrength(peak_collection);
    }

    if (_filter_flags->d_range) {
        ohklLog(Level::Info, "Filtering by D-range");
        filterDRange(peak_collection);
    }

    if (_filter_flags->extinct) {
        ohklLog(Level::Info, "Filtering out extinct peaks");
        filterExtinct(peak_collection);
    }

    if (_filter_flags->sparse) {
        ohklLog(Level::Info, "Filtering sparse data set");
        filterSparseDataSet(peak_collection);
    }

    if (_filter_flags->significance) {
        ohklLog(Level::Info, "Filtering by significance");
        filterSignificance(peak_collection);
    }

    if (_filter_flags->overlapping) {
        ohklLog(Level::Info, "Filtering out overlapping peaks");
        filterOverlapping(peak_collection);
    }

    if (_filter_flags->complementary) { // this does nothing
        filterComplementary(peak_collection);
        ohklLog(Level::Info, "Filtering complementary");
    }

    if (_filter_flags->frames) {
        filterFrameRange(peak_collection);
        ohklLog(
            Level::Info, "Filtering peaks from frames in range", _filter_params->first_frame, "-",
            _filter_params->last_frame);
    }

    if (_filter_flags->rejection_flag) {
        filterRejectionFlag(peak_collection);
        ohklLog(
            Level::Info,
            "Filtering by rejection flag: ", static_cast<int>(_filter_params->rejection_flag));
    }

    if (_filter_flags->intensity) {
        filterIntensity(peak_collection);
        ohklLog(Level::Info, "Filtering by intensity");
    }

    if (_filter_flags->sigma) {
        filterSigma(peak_collection);
        ohklLog(Level::Info, "Filtering by sigma");
    }

    if (_filter_flags->gradient) {
        filterGradient(peak_collection);
        ohklLog(Level::Info, "Filtering by background gradient");
    }

    if (_filter_flags->gradient_sigma) {
        filterGradientSigma(peak_collection);
        ohklLog(Level::Info, "Filtering by background gradient sigma");
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

Intensity PeakFilter::peakIntensity(Peak3D* peak) const
{
    if (_filter_params->sum_intensities)
        return peak->correctedSumIntensity();
    else
        return peak->correctedProfileIntensity();
}

} // namespace ohkl
