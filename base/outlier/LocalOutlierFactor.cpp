//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      base/outlier/LocalOutlierFactor.cpp
//! @brief     Implements class LocalOutlierFactor
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "base/outlier/LocalOutlierFactor.h"
#include "base/outlier/Knn.h"

namespace ohkl {

LocalOutlierFactor::LocalOutlierFactor(
    unsigned int dimension, unsigned int k, bool normalise_data = true)
    : Knn(dimension, k)
    , _hasBounds(false)
    , _isNormalised(false)
    , _threshold(1.0)
{
    if (normalise_data)
        normalise();
}

void LocalOutlierFactor::computeBounds()
{
    // for (std::size_t idx = 0; idx < _npoints; ++idx) {
    //     for (std::size_t dim = 0; dim < _dimension; ++dim) {
    //         _lower(dim) = std::min(_lower(dim), (*_points)(idx, dim));
    //         _upper(dim) = std::max(_upper(dim), (*_points)(idx, dim));
    //     }
    // }
    _hasBounds = true;
}

void LocalOutlierFactor::normalise()
{
    if (!_hasBounds)
        computeBounds();
    if (_isNormalised)
        return;
    // for (std::size_t idx = 0; idx < _npoints; ++idx) {
    //     for (std::size_t dim = 0; dim < _dimension; ++dim) {
    //         (*_points)(idx, dim) =
    //             ((*_points)(idx, dim) - _lower(dim)) / (_upper(dim) - _lower(dim));
    //     }
    // }
    _isNormalised = true;
}

double LocalOutlierFactor::localReachabilityDensity(std::vector<double> point)
{
    KnnResult knn = queryPoint(point);

    int n_neighbours = 0;
    double sum_reachability = 0.0;
    for (std::size_t idx = 0; idx < _k; ++idx) {
        int neighbour_idx = knn.indices[idx];
        std::vector<double> neighbour;
        const cv::Mat& point = getPoint(neighbour_idx);
        for (std::size_t col = 0; col < _points.cols; ++col)
            neighbour.push_back(getPoint(neighbour_idx)[col]);
        sum_reachability += reachabilityDistance(point_idx, neighbour_idx);
        ++n_neighbours;
    }

    return static_cast<double>(n_neighbours) / sum_reachability;
}

double LocalOutlierFactor::localOutlierFactor(std::vector<double> point)
{
    kDistance(point_idx);
    double point_lrd = localReachabilityDensity(point_idx);

    int n_neighbours = 0;
    double sum_lrd = 0.0;
    for (std::size_t idx = 0; idx < _k; ++idx) {
        int neighbour_idx = _neighbours(point_idx, idx);
        if (neighbour_idx == -1)
            continue;
        double neighbour_lrd = localReachabilityDensity(neighbour_idx);
        sum_lrd +=  neighbour_lrd / point_lrd;
        ++n_neighbours;
    }
    // What if n_neighbours == 0?
    return sum_lrd / n_neighbours;
}

std::vector<std::pair<double, int>> LocalOutlierFactor::findOutliers()
{
    std::vector<std::pair<double, int>> outliers;
    for (std::size_t idx = 0; idx < _npoints; ++idx) {
        double lof = localOutlierFactor(idx);
        if (lof > _threshold)
            outliers.emplace_back(std::pair<double, int>(lof, idx));
    }
    std::sort(outliers.begin(), outliers.end());
    return outliers;
}

} // namespace ohkl
