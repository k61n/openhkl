//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/shape/Profile1D.h
//! @brief     Defines class Profile1D
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_CORE_SHAPE_PROFILE1D_H
#define OHKL_CORE_SHAPE_PROFILE1D_H

#include <vector>

#include "core/peak/Intensity.h"

namespace ohkl {

/*! \addtogroup python_api
 *  @{*/

/*! \brief A 1D peak profile used for profile-fitting integration
 *
 *  A profile is the average of many peaks. This class allows profiles to be
 *  summed and normalised for use in integration.
 */
class Profile1D {
 public:
    //! Constructor. sigma_max indicates maximum number of standard deviations
    Profile1D(
        const Intensity& mean_background = Intensity(), double sigma_max = 4.0, size_t num = 200);
    //! Add a data point to the bins.
    //! @param r2 : equal to \f$(x-x0)\cdot A\cdot (x-x0)\f$ where \f$A\f$ is the inverse
    //! covariance matrix of the peak.
    //! @param M : the total count (no background correction)
    void addPoint(double r2, double M);
    //! Returns the vector of integrated counts values
    const std::vector<double>& counts() const;
    //! Returns the number of points in each bin
    const std::vector<int>& npoints() const;
    //! Returns the profile \f$I(s) / I(s_{max})\f$
    std::vector<Intensity> profile() const;
    //! Reset the profile to zero
    void reset();

 private:
    std::vector<double> _counts;
    std::vector<int> _npoints;
    std::vector<double> _endpoints;
    Intensity _meanBkg;
};

/*! @}*/
} // namespace ohkl

#endif // OHKL_CORE_SHAPE_PROFILE1D_H
