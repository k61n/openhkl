//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      base/outlier/Knn.h
//! @brief     Defines class Knn
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_BASE_OUTLIER_KNN_H
#define OHKL_BASE_OUTLIER_KNN_H

#include <opencv2/flann/miniflann.hpp>

#include <vector>

namespace ohkl {

struct KnnResult {
    std::vector<int> indices;
    std::vector<double> distances;
};

/*! \addtogroup python_api
 *  @{*/

/*! \brief Compute k-nearest neighbours for a set of n-dimensional data
 *
 *  Given a npoints x n dimensional matrix of data points, compute the nearest k
 *  neighbours using a given distance function. This is a wrapper for the OpenCV
 * FLANN (Fast Library for Approximate Nearest Neighbours) k-D tree/KNN implementation
 */

class Knn {
 public:
    Knn(unsigned int dimension, unsigned int k);

    template<class PointT>
    void addPoints(const std::vector<PointT>& points) {
        cv::flann::KDTreeIndexParams index_params;
        _points = cv::Mat(points);
        _kdtree{_points.reshape(1), index_params};
    };

    KnnResult queryPoint(std::vector<double> point);

    //! Compute the k-distance for a point
    double kDistance(std::vector<double> point);
    //! Compute the reachability distance for a pair of points
    double reachabilityDistance(std::vector<double> point1, std::vector<double> point2);

 protected:
    //! Get point with given index
    cv::Mat getPoint(std::size_t idx) { return _points.row(idx); };

    //! Dimensionality of data
    unsigned int _dimension;
    //! Number of nearest neighbours
    unsigned int _k;

    //! The OpenCV KNN object
    cv::flann::Index _kdtree;
    //! OpenCV matrix containing the points
    cv::Mat _points;
};

} // namespace ohkl

#endif // OHKL_BASE_OUTLIER_KNN_H
