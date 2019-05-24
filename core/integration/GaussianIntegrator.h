#ifndef CORE_INTEGRATION_GAUSSIANINTEGRATOR_H
#define CORE_INTEGRATION_GAUSSIANINTEGRATOR_H

#include <Eigen/Dense>

#include "IPeakIntegrator.h"

namespace nsx {

class DataSet;

//! \brief Compute integrated intensity by fitting to an analytic 3d Gaussian.
class GaussianIntegrator : public IPeakIntegrator {
public:
    GaussianIntegrator(bool fit_center, bool fit_cov);
    bool compute(sptrPeak3D peak, const IntegrationRegion& region) override;
    //! Return the analytic profile computed over the given integration region
    std::vector<double> profile(sptrPeak3D peak, const IntegrationRegion& region);

private:
    bool _fitCenter;
    bool _fitCov;
};

} // end namespace nsx

#endif // CORE_INTEGRATION_GAUSSIANINTEGRATOR_H
