#include <map>

#include "DataSet.h"
#include "Diffractometer.h"
#include "MergedData.h"
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
        auto cell = peak->activeUnitCell();

        // no unit cell assigned:
        if (!cell) {
            return true;
        }

        // try to index
        Eigen::RowVector3d hkl;
        auto q = peak->getQ();
        if (!cell->getMillerIndices(q, hkl)) {
            return true;
        }
    }
   
    if (filter._removeMasked) {
        if (peak->isMasked()) {
            return true;
        }
    }

    if (filter._removeUnselected) {
        if (!peak->isSelected()) {
            return true;
        }
    }

    if (filter._removeIsigma) {
        nsx::Intensity i = peak->getCorrectedIntensity();
        if (i.value() / i.sigma() < filter._Isigma) {
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

    auto q = peak->getQ().rowVector();
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

PeakSet PeakFilter::apply(const PeakSet& reference_peaks) const
{
    std::vector<Ellipsoid> ellipsoids;
    std::vector<sptrPeak3D> peaks;
    std::set<sptrPeak3D> bad_peaks;
    std::set<sptrPeak3D> good_peaks;
    std::set<Octree::collision_pair> collisions;
    std::set<sptrUnitCell> crystals;
    Eigen::Vector3d lower(1e100, 1e100, 1e100);
    Eigen::Vector3d upper(-1e100, -1e100, -1e100);

    for (auto peak: reference_peaks) {
        ellipsoids.emplace_back(peak->getShape());
        peaks.emplace_back(peak);
        auto cell = peak->activeUnitCell();

        if (cell) {
            crystals.insert(cell);
        }

        Eigen::Vector3d p = peak->getShape().center();

        for (int i = 0; i < 3; ++i) {
            lower(i) = std::min(lower(i), p(i));
            upper(i) = std::max(upper(i), p(i));
        }
    }

    for (auto peak: peaks) {
        if (invalid(*this, peak)) {
            bad_peaks.insert(peak);
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
            bad_peaks.insert(peaks[i]);
            bad_peaks.insert(peaks[j]);
        }
    }

    for (auto cell: crystals) {     
        SpaceGroup group(cell->spaceGroup());
        MergedData merged(group, true);

        for (auto peak: peaks) {
            if (peak->activeUnitCell() != cell) {
                continue;
            }

            merged.addPeak(peak);

            auto q = peak->getQ();
            Eigen::RowVector3i hkl = cell->getIntegerMillerIndices(q);
            if (_removeForbidden && group.isExtinct(hkl(0), hkl(1), hkl(2))) {
                bad_peaks.insert(peak);
            }
        }

        for (auto&& merged_peak: merged.getPeaks()) {
            // p value too high: reject peaks
            if (_removeMergedP && merged_peak.pValue() > _mergedP) {
                for (auto&& p: merged_peak.getPeaks()) {
                    bad_peaks.insert(p);
                }
            }
        }
    }

    for (auto it = peaks.begin(); it != peaks.end(); ) {
        auto jt = bad_peaks.find(*it);
        if (jt != bad_peaks.end()) {
            it = peaks.erase(it);
            bad_peaks.erase(jt);
        } else {
            good_peaks.insert(*it);
            ++it;
        }        
    }

    return good_peaks;
}

} // end namespace nsx
