//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/integration/GaussianIntegrator.h
//! @brief     Defines ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef CORE_INTEGRATION_GAUSSIANINTEGRATOR_H
#define CORE_INTEGRATION_GAUSSIANINTEGRATOR_H

#include <Eigen/Dense>

#include "core/integration/IPeakIntegrator.h"

namespace nsx {

class DataSet;

//! \brief Compute integrated intensity by fitting to an analytic 3d Gaussian.
class GaussianIntegrator : public IPeakIntegrator {
public:
    GaussianIntegrator(bool fit_center, bool fit_cov);
    bool compute(sptrPeak3D peak, const IntegrationRegion& region) override;
    //! Returns the analytic profile computed over the given integration region
    std::vector<double> profile(sptrPeak3D peak, const IntegrationRegion& region);

private:
    bool _fitCenter;
    bool _fitCov;
};

} // end namespace nsx

#endif // CORE_INTEGRATION_GAUSSIANINTEGRATOR_H
