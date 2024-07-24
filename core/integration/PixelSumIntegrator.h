//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/integration/PixelSumIntegrator.h
//! @brief     Defines class PixelSumIntegrator
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_CORE_INTEGRATION_PIXELSUMINTEGRATOR_H
#define OHKL_CORE_INTEGRATION_PIXELSUMINTEGRATOR_H

#include "core/integration/IIntegrator.h"

namespace ohkl {

/*! \addtogroup python_api
 *  @{*/

/*! \brief Peak integration using naive background estimation and subtraction.
 */
class PixelSumIntegrator : public IIntegrator {
 public:
    //! Construct the pixel sum integrator
    //! @param fit_center update the peak center as part of integration
    //! @param fit_covariance update the peak shape covariance matrix as part of integration
    PixelSumIntegrator();

 protected:
    //! Integrate a peak
    ComputeResult compute(Peak3D* peak, ShapeModel*, const IntegrationRegion& region) override;
};

/*! @}*/
} // namespace ohkl

#endif // OHKL_CORE_INTEGRATION_PIXELSUMINTEGRATOR_H
