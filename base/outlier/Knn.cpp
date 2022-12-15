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

Knn::Knn(unsigned int dimension, unsigned int k)
    : _dimension(dimension)
    , _k(k)
{
}

KnnResult Knn::queryPoint(std::vector<double> point)
{
    KnnResult result;
    _kdtree.knnSearch(point, result.indices, result.distances, _k);
    return result;
}

double Knn::kDistance(std::vector<double> point)
{
    KnnResult result = queryPoint(point);
    return result.distances[_k-1];
}

double Knn::reachabilityDistance(std::vector<double> point1, std::vector<double> point2)
{
    double kdist =  kDistance(point1);
    double dist;
    for (std::size_t idx = 0; idx < point1.size(); ++idx) {
        double diff = point1[idx] - point2[idx];
        dist += diff * diff;
    }
    dist = std::sqrt(dist);
    return std::max(kdist, dist);
}

void Knn::computeBounds()
{

}

} // namespace ohkl
