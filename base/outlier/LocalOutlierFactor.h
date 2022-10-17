//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      base/outlier/LocalOutlierFactor.h
//! @brief     Defines class LocalOutlierFactor
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_BASE_OUTLIER_LOCALOUTLIERFACTOR_H
#define OHKL_BASE_OUTLIER_LOCALOUTLIERFACTOR_H

#include "base/outlier/Knn.h"

namespace ohkl {

/*! \addtogroup python_api
 *  @{*/

/*! \brief Compute local outlier factors for a set of n-dimensional data
 *
 *  Given a npoints x n dimensional matrix of data points, determine outliers using
 *  local outlier factor (https://www.dbs.ifi.lmu.de/Publikationen/Papers/LOF.pdf).
 */

class LocalOutlierFactor : public Knn {
 public:
    LocalOutlierFactor(
        unsigned int k, unsigned int dimension, unsigned int npoints, Eigen::MatrixXd* points,
        bool normalise_data = true);

    //! Return a vector of outliers
    std::vector<std::pair<double, int>> findOutliers();

 private:
    //! Compute the bounds for each dimension
    void computeBounds();
    //! Normalise the data according to the computed bounds
    void normalise();

    //! Compute local reachability density for a given point
    double localReachabilityDensity(unsigned int point_idx);
    //! Compute local outlier factor for a given point
    double localOutlierFactor(unsigned int point_idx);

    //! Lower bound of the data
    Eigen::VectorXd _lower;
    //! Upper bound of the data
    Eigen::VectorXd _upper;
    //! Whether bounds have been computed
    bool _hasBounds;
    //! Whether data has been normalised
    bool _isNormalised;
    //! Rejection threshold for outliers
    double _threshold;
};

/*! @}*/
} // namespace ohkl

#endif // OHKL_BASE_OUTLIER_LOCALOUTLIERFACTOR_H
