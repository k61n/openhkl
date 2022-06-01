//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/integration/GaussianIntegrator.h
//! @brief     Defines class GaussianIntegrator
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef NSX_CORE_INTEGRATION_GAUSSIANINTEGRATOR_H
#define NSX_CORE_INTEGRATION_GAUSSIANINTEGRATOR_H

#include "core/shape/IPeakIntegrator.h"

namespace nsx {

/*! \addtogroup python_api
 *  @{*/

/*! \brief Compute integrated intensity by fitting to an analytic 3D Gaussian.*/
class GaussianIntegrator : public IPeakIntegrator {
 public:
    GaussianIntegrator(bool fit_center, bool fit_cov);
    //! Integrate a peak
    bool compute(
        Peak3D* peak, ShapeModel* shape_model, const IntegrationRegion& region) override;
    //! Returns the analytic profile computed over the given integration region
    std::vector<double> profile(Peak3D* peak, const IntegrationRegion& region);
};

/*! @}*/
} // namespace nsx

#endif // NSX_CORE_INTEGRATION_GAUSSIANINTEGRATOR_H
