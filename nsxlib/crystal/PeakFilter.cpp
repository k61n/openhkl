#include <map>

#include "PeakFilter.h"
#include "Peak3D.h"
#include "../data/DataSet.h"
#include "../geometry/Octree.h"

namespace {

bool invalid(const nsx::PeakFilter& filter, nsx::sptrDataSet data, nsx::sptrPeak3D peak)
{
    if (filter._removeUnindexed) {
        // todo
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

    if (filter._removeMultiplyIndexed) {
        // todo
    }

    if (filter._removeIsigma) {
        nsx::Intensity i = peak->getCorrectedIntensity();
        if (i.getValue() / i.getSigma() < filter._Isigma) {
            return true;
        }
    }

    if (filter._removeSignificance) {
        // todo
    }

    if (filter._removeOverlapping) {
        // note: this is a special case handled in PeakFilter::apply
    }

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

    return npeaks - data->getPeaks().size();
}

} // end namespace nsx
