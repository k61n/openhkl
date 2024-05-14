//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/integration/GaussianIntegrator.h
//! @brief     Defines class GaussianIntegrator
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_CORE_INTEGRATION_GAUSSIANINTEGRATOR_H
#define OHKL_CORE_INTEGRATION_GAUSSIANINTEGRATOR_H

#include "core/integration/IIntegrator.h"

namespace ohkl {

/*! \addtogroup python_api
 *  @{*/

/*! \brief Compute integrated intensity by fitting to an analytic 3D Gaussian.*/
class GaussianIntegrator : public IIntegrator {
 public:
    GaussianIntegrator();

 protected:
    //! Integrate a peak
    ComputeResult compute(
        Peak3D* peak, ShapeModel* shape_model, const IntegrationRegion& region) override;
    //! Returns the analytic profile computed over the given integration region
    std::vector<double> profile(Peak3D* peak, const IntegrationRegion& region);
};

/*! @}*/
} // namespace ohkl

#endif // OHKL_CORE_INTEGRATION_GAUSSIANINTEGRATOR_H
