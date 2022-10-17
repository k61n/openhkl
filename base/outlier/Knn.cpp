//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      base/outlier/Knn.cpp
//! @brief     Implements class Knn
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "base/outlier/Knn.h"

#include <vector>

namespace ohkl {

Knn::Knn(unsigned int k, unsigned int dimension, unsigned int npoints, Eigen::MatrixXd* points)
    : _distance(dimension)
    , _k(k)
    , _dimension(dimension)
    , _npoints(npoints)
    , _points(points)
{
    _neighbours = Eigen::MatrixXi::Constant(npoints, k, -1);
    _kDistances = Eigen::VectorXd::Zero(npoints);
    _hasDistance = Eigen::VectorXi::Zero(npoints);
}

void Knn::kDistance(unsigned int point_index)
{
    if (_hasDistance(point_index) != 0) // Make sure we only do the computation once
        return;

    // Compute distances from the point with the given index
    std::vector<std::pair<double, int>> distances;
    Eigen::VectorXd point = _points->row(point_index);
    for (std::size_t idx = 0; idx < _npoints; ++idx) {
        if (idx == point_index) // skip the point itself
            continue;
        for (std::size_t dim = 0; dim < _dimension; ++dim) {
            double dist = _distance.euclidean(point, _points->row(idx));
            distances.emplace_back(std::pair<double, int>(dist, idx));
        }
    }

    std::sort(distances.begin(), distances.end());

    // Store indices of neighbours
    for (std::size_t idx = 0; idx < _k; ++idx) {
        int label = distances.at(idx).second;
        _neighbours(point_index, idx) = label;
    }

    // k-distance is the kth distance
    _kDistances(point_index) =
        distances.size() <= _k ? distances[_k-1].first : distances.back().first;
    _hasDistance(point_index) = 1;
}

double Knn::reachabilityDistance(unsigned int point1_idx, unsigned int point2_idx)
{
    kDistance(point1_idx);
    double dist = _distance.euclidean(_points->row(point1_idx), _points->row(point2_idx));
    return std::max(_kDistances(point1_idx), dist);
}

} // namespace ohkl
