#include <algorithm>
#include <iterator>
#include <map>
#include <set>

#include "DataSet.h"
#include "Diffractometer.h"
#include "MergedData.h"
#include "MillerIndex.h"
#include "Octree.h"
#include "Peak3D.h"
#include "PeakFilter.h"
#include "ReciprocalVector.h"
#include "Sample.h"
#include "SpaceGroup.h"
#include "UnitCell.h"

namespace {


bool invalid(const nsx::PeakFilter& filter, nsx::sptrPeak3D peak)
{
    if (filter._removeUnindexed) {
        auto cell = peak->unitCell();

        // no unit cell assigned:
        if (!cell) {
            return true;
        }

        // try to index
        nsx::MillerIndex hkl(peak->q(), *cell);
        if (!hkl.indexed(cell->indexingTolerance())) {
            return true;
        }
    }
   
    if (filter._removeUnselected) {
        if (!peak->enabled()) {
            return true;
        }
    }

    if (filter._removeIsigma) {
        nsx::Intensity i = peak->correctedIntensity();
        // note: we use negated condition so that nan/inf automatically get rejected
        if (!(i.value() > filter._Isigma * i.sigma())) {
            return true;
        }
    }

    if (filter._removePValue) {
        if (peak->pValue() > filter._pvalue) {
            return true;
        }
    }

    // note: _removeOverlapping is a special case handled in PeakFilter::apply

    // note: _removeForbidden is a special case handled in PeakFilter::apply

    // note: merged peaks are handled separately    

    auto q = peak->q().rowVector();
    const double d = 1.0 / q.norm();

    if (filter._removeDmin) {
        if (d < filter._dmin) {
            return true;
        }
    }

    if (filter._removeDmax) {
        if (d > filter._dmax) {
            return true;
        }
    }

    // failed to violate any condition
    return false;
}

} // end anonymous namespace

namespace nsx {

PeakList PeakFilter::apply(const PeakList& reference_peaks) const
{
    PeakList peaks;
    PeakList bad_peaks;
    PeakList good_peaks;

    std::vector<Ellipsoid> ellipsoids;
    std::set<Octree::collision_pair> collisions;
    std::set<sptrUnitCell> crystals;
    Eigen::Vector3d lower(1e100, 1e100, 1e100);
    Eigen::Vector3d upper(-1e100, -1e100, -1e100);

    for (auto peak: reference_peaks) {
        ellipsoids.emplace_back(peak->shape());
        peaks.push_back(peak);
        auto cell = peak->unitCell();

        if (cell) {
            crystals.insert(cell);
        }

        Eigen::Vector3d p = peak->shape().center();

        for (int i = 0; i < 3; ++i) {
            lower(i) = std::min(lower(i), p(i));
            upper(i) = std::max(upper(i), p(i));
        }
    }

    for (auto peak: peaks) {
        if (invalid(*this, peak)) {
            bad_peaks.push_back(peak);
        }
    }

    if (_removeOverlapping) {
        // build octree
        Octree tree(lower, upper);

        for (unsigned int i = 0; i < peaks.size(); ++i) {
            tree.addData(&ellipsoids[i]);
        }

        collisions = tree.getCollisions();

        // handle collisions below
        for (auto collision: collisions) {
            unsigned int i = collision.first - &ellipsoids[0];
            unsigned int j = collision.second - &ellipsoids[0];
            bad_peaks.push_back(peaks[i]);
            bad_peaks.push_back(peaks[j]);
        }
    }

    for (auto cell: crystals) {     
        SpaceGroup group(cell->spaceGroup());
        MergedData merged(group, true);

        PeakFilter peak_filter;
        PeakList filtered_peaks;
        filtered_peaks = peak_filter.unitCell(peaks,cell);
        filtered_peaks = peak_filter.indexed(filtered_peaks,cell,cell->indexingTolerance());

        for (auto peak : filtered_peaks) {

            merged.addPeak(peak);

            MillerIndex hkl(peak->q(), *cell);

            if (_removeForbidden && group.isExtinct(hkl)) {
                bad_peaks.push_back(peak);
            }
        }

        for (auto&& merged_peak: merged.peaks()) {
            // p value too high: reject peaks
            if (_removeMergedP && merged_peak.pValue() > _mergedP) {
                for (auto&& p: merged_peak.peaks()) {
                    bad_peaks.push_back(p);
                }
            }
        }
    }

    for (auto it = peaks.begin(); it != peaks.end(); ) {
        auto jt = std::find(bad_peaks.begin(),bad_peaks.end(),*it);
        if (jt != bad_peaks.end()) {
            it = peaks.erase(it);
            bad_peaks.erase(jt);
        } else {
            good_peaks.push_back(*it);
            ++it;
        }        
    }

    return good_peaks;
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


PeakList PeakFilter::enabled(const PeakList& peaks, bool selection_flag) const
{
    PeakList filtered_peaks;

    std::copy_if(peaks.begin(),peaks.end(),std::back_inserter(filtered_peaks),[selection_flag](sptrPeak3D peak){return selection_flag == peak->enabled();});

    return filtered_peaks;
}

PeakList PeakFilter::indexed(const PeakList& peaks, sptrUnitCell cell, double tolerance) const
{
    PeakList filtered_peaks;

    for (auto peak : peaks) {
        if (!cell) {
            continue;
        }
        MillerIndex miller_index(peak->q(), *cell);
        if (miller_index.indexed(tolerance)) {
            filtered_peaks.push_back(peak);
        }
    }

    return filtered_peaks;
}

PeakList PeakFilter::indexed(const PeakList& peaks, double tolerance) const
{
    PeakList filtered_peaks;

    for (auto peak : peaks) {
        auto cell = peak->unitCell();
        if (!cell) {
            continue;
        }
        MillerIndex miller_index(peak->q(), *cell);
        if (miller_index.indexed(tolerance)) {
            filtered_peaks.push_back(peak);
        }
    }

    return filtered_peaks;
}

PeakList PeakFilter::dataset(const PeakList& peaks, sptrDataSet dataset) const
{
    PeakList filtered_peaks;

    for (auto peak : peaks) {
        if (peak->data() == dataset) {
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

PeakList PeakFilter::minSigma(const PeakList& peaks, double threshold) const
{
    PeakList filtered_peaks;

    for (auto peak : peaks) {

        auto corrected_intensity = peak->correctedIntensity();

        double sigma = corrected_intensity.sigma();

        if (sigma > threshold) {
            filtered_peaks.push_back(peak);
        }
    }

    return filtered_peaks;
}

PeakList PeakFilter::signalToNoise(const PeakList& peaks, double threshold) const
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

        if (i_over_sigma > threshold) {
            filtered_peaks.push_back(peak);
        }
    }

    return filtered_peaks;
}

PeakList PeakFilter::lowIntensity(const PeakList& peaks, double threshold) const
{
    PeakList filtered_peaks;

    for (auto peak : peaks) {

        auto intensity = peak->correctedIntensity();

        if (intensity.value() < threshold) {
            filtered_peaks.push_back(peak);
        }
    }

    return filtered_peaks;
}

PeakList PeakFilter::predicted(const PeakList& peaks) const
{
    PeakList filtered_peaks;

    for (auto peak : peaks) {

        if (peak->isPredicted()) {
            filtered_peaks.push_back(peak);
        }
    }

    return filtered_peaks;
}

PeakList PeakFilter::dRange(const PeakList& peaks, double dmin, double dmax) const
{
    PeakList filtered_peaks;

    for (auto peak : peaks) {

        auto q = peak->q();

        double d = 1.0/q.rowVector().norm();

        if (d >= dmin && d <= dmax) {
            filtered_peaks.push_back(peak);
        }
    }

    return filtered_peaks;
}

PeakList PeakFilter::dMin(const PeakList& peaks, double dmin) const
{
    PeakList filtered_peaks;

    for (auto peak : peaks) {

        auto q = peak->q();

        double d = 1.0/q.rowVector().norm();

        if (d >= dmin) {
            filtered_peaks.push_back(peak);
        }
    }

    return filtered_peaks;
}

PeakList PeakFilter::dMax(const PeakList& peaks, double dmax) const
{
    PeakList filtered_peaks;

    for (auto peak : peaks) {

        auto q = peak->q();

        double d = 1.0/q.rowVector().norm();

        if (d <= dmax) {
            filtered_peaks.push_back(peak);
        }
    }

    return filtered_peaks;
}

PeakList PeakFilter::significance(const PeakList& peaks, double p_value_threshold) const
{
    PeakList filtered_peaks;

    for (auto peak : peaks) {

        const double p_value = peak->pValue();

        if (p_value <= p_value_threshold) {
            filtered_peaks.push_back(peak);
        }
    }

    return filtered_peaks;
}

PeakList PeakFilter::selection(const PeakList& peaks, const std::vector<int>& indexes) const
{
    PeakList filtered_peaks;

    for (int idx: indexes) {
        if (idx <0 || idx >= peaks.size()) {
            continue;
        }
        filtered_peaks.push_back(peaks[idx]);
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
