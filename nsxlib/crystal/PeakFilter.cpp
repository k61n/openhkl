#include <map>

#include "PeakFilter.h"
#include "Peak3D.h"

#include "../crystal/UnitCell.h"
#include "../crystal/SpaceGroup.h"
#include "../data/DataSet.h"
#include "../data/MergedData.h"
#include "../geometry/Octree.h"
#include "../instrument/Diffractometer.h"
#include "../instrument/Sample.h"

namespace {

bool invalid(const nsx::PeakFilter& filter, nsx::sptrDataSet data, nsx::sptrPeak3D peak)
{
    if (filter._removeUnindexed) {
        auto cell = peak->getActiveUnitCell();

        // no unit cell assigned:
        if (!cell) {
            return true;
        }

        // try to index
        Eigen::RowVector3d hkl;
        if (!peak->getMillerIndices(*cell, hkl)) {
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
        if (i.getValue() / i.getSigma() < filter._Isigma) {
            return true;
        }
    }

    if (filter._removePValue) {
        if (peak->pValue() < filter._pvalue) {
            return true;
        }
    }

    // note: _removeOverlapping is a special case handled in PeakFilter::apply

    // note: _removeForbidden is a special case handled in PeakFilter::apply

    // note: merged peaks are handled separately    

    const double d = 1.0 / peak->getQ().norm();

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

PeakFilter::PeakFilter()
{

}

PeakFilter::~PeakFilter()
{

}

int PeakFilter::apply(sptrDataSet data) const
{
    std::vector<Ellipsoid> ellipsoids;
    std::vector<sptrPeak3D> peaks;
    std::set<Octree::collision_pair> collisions;
    Eigen::Vector3d lower(1e100, 1e100, 1e100);
    Eigen::Vector3d upper(-1e100, -1e100, -1e100);

    for (auto peak: data->getPeaks()) {
        ellipsoids.emplace_back(peak->getShape());
        peaks.emplace_back(peak);

        Eigen::Vector3d p = peak->getShape().center();

        for (int i = 0; i < 3; ++i) {
            lower(i) = std::min(lower(i), p(i));
            upper(i) = std::max(upper(i), p(i));
        }
    }

    const unsigned int npeaks = peaks.size();

    for (unsigned int i = 0; i < npeaks; ++i) {
        if (invalid(*this, data, peaks[i])) {
            data->removePeak(peaks[i]);
        }
    }

    if (!_removeOverlapping) {
        return npeaks - data->getPeaks().size();
    }

    // build octree
    Octree tree(lower, upper);

    for (unsigned int i = 0; i < npeaks; ++i) {
        tree.addData(&ellipsoids[i]);
    }

    collisions = tree.getCollisions();

    // handle collisions below
    for (auto collision: collisions) {
        unsigned int i = collision.first - &ellipsoids[0];
        unsigned int j = collision.second - &ellipsoids[0];
        assert(i < npeaks && j < npeaks);
        data->removePeak(peaks[i]);
        data->removePeak(peaks[j]);
    }

    if (!_removeForbidden) {
        return npeaks - data->getPeaks().size();
    }

    for (auto i = 0; data->getDiffractometer()->getSample()->getNCrystals(); ++i) {
        auto cell = data->getDiffractometer()->getSample()->getUnitCell(i);
        SpaceGroup group(cell->getSpaceGroup());
        MergedData merged(group, true);

        for (auto peak: peaks) {
            if (peak->getActiveUnitCell() != cell) {
                continue;
            }

            PeakCalc pcalc(*peak);
            merged.addPeak(pcalc);

            Eigen::RowVector3i hkl = peak->getIntegerMillerIndices();
            if (group.isExtinct(hkl(0), hkl(1), hkl(2))) {
                data->removePeak(peak);
            }
        }

        if (!_removeMergedP) {
            continue;
        }

        #if 0
        for (auto&& merged_peak: merged.getPeaks()) {
            // p value too high: reject peaks
            if (merged_peak.pValue() > _mergedP) {
                for (auto&& p: merged_peak.getPeaks()) {
                    data->removePeak(p);
                }
            }
        }
        #endif
    }

    return npeaks - data->getPeaks().size();
}

} // end namespace nsx
