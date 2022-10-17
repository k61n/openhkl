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

#include "base/outlier/Distance.h"

#include <vector>

namespace ohkl {

/*! \addtogroup python_api
 *  @{*/

/*! \brief Compute k-nearest neighbours for a set of n-dimensional data
 *
 *  Given a npoints x n dimensional matrix of data points, compute the nearest k
 *  neighbours using a given distance function
 */

class Knn {
 public:
    Knn(unsigned int k, unsigned int dimension, unsigned int npoints, Eigen::MatrixXd* points);

 protected:
    //! Compute the k-distance for a point
    void kDistance(unsigned int point_index);
    //! Compute the reachability distance for a pair of points
    double reachabilityDistance(unsigned int point1_idx, unsigned int point2_idx);

    //! Class for calculating the given distance metric
    Distance _distance;
    //! Number of nearest neighbours
    unsigned int _k;
    //! Dimensionality of data
    unsigned int _dimension;
    //! Number of points in data set
    unsigned int _npoints;
    //! Pointer to the data (_npoints x _dimension matrix)
    Eigen::MatrixXd* _points;
    //! Indices of the k nearest neighbours
    Eigen::MatrixXi _neighbours;
    //! Vector of k distances
    Eigen::VectorXd _kDistances;
    //! Flag determining whether the distance for a specific point has been computed
    Eigen::VectorXi _hasDistance;
};

} // namespace ohkl

#endif // OHKL_BASE_OUTLIER_KNN_H
