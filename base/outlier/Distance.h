//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      base/outlier/Distance.h
//! @brief     Defines class Distance
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_BASE_OUTLIER_DISTANCE_H
#define OHKL_BASE_OUTLIER_DISTANCE_H

#include <Eigen/Dense>
#include <Eigen/src/Core/Matrix.h>

namespace ohkl {

/*! \addtogroup python_api
 *  @{*/

/*! \brief Compute distance between two n-dimensional points for a given metric
 *
 *  Container class for computing distances between n-dimensional points.
 */

class Distance  {

 public:
    Distance(unsigned int dimension);

    //! Euclidean distance
    double euclidean(Eigen::VectorXd point1, Eigen::VectorXd point2);
    //! Manhattan distance
    double manhattan(Eigen::VectorXd point1, Eigen::VectorXd point2);
    //! Cosine distance
    double cosine(Eigen::VectorXd point1, Eigen::VectorXd point2);

 private:
    unsigned int _dimension;

};

} // namespace ohkl

#endif // OHKL_BASE_OUTLIER_DISTANCE_H
