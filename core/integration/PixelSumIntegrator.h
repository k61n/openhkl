//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/integration/PixelSumIntegrator.h
//! @brief     Defines class PixelSumIntegrator
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef NSX_CORE_INTEGRATION_PIXELSUMINTEGRATOR_H
#define NSX_CORE_INTEGRATION_PIXELSUMINTEGRATOR_H

#include "core/integration/MeanBackgroundIntegrator.h"

namespace nsx {

/*! \addtogroup python_api
 *  @{*/

/*! \brief Peak integration using naive background estimation and subtraction.
 */
class PixelSumIntegrator : public MeanBackgroundIntegrator {
 public:
    //! Construct the pixel sum integrator
    //! @param fit_center update the peak center as part of integration
    //! @param fit_covariance update the peak shape covariance matrix as part of integration
    PixelSumIntegrator(bool fit_center, bool fit_covariance);
    ~PixelSumIntegrator();
    //! Integrate a peak
    bool compute(Peak3D* peak, ShapeModel* shape_model, const IntegrationRegion& region) override;
};

/*! @}*/
} // namespace nsx

#endif // NSX_CORE_INTEGRATION_PIXELSUMINTEGRATOR_H
