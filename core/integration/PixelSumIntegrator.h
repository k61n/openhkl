//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
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

#ifndef CORE_INTEGRATION_PIXELSUMINTEGRATOR_H
#define CORE_INTEGRATION_PIXELSUMINTEGRATOR_H

#include "core/integration/MeanBackgroundIntegrator.h"

namespace nsx {

//! Peak integration using naive background estimation and subtraction.

class PixelSumIntegrator : public MeanBackgroundIntegrator {
public:
    //! Construct the pixel sum integrator
    //! \param fit_center update the peak center as part of integration
    //! \param fit_covariance update the peak shape covariance matrix as part of integration
    PixelSumIntegrator(bool fit_center, bool fit_covariance);
    ~PixelSumIntegrator();
    bool compute(sptrPeak3D peak, const IntegrationRegion& region) override;

private:
    bool _fitCenter;
    bool _fitCovariance;
};

} // namespace nsx

#endif // CORE_INTEGRATION_PIXELSUMINTEGRATOR_H
