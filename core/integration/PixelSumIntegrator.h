#ifndef CORE_INTEGRATION_PIXELSUMINTEGRATOR_H
#define CORE_INTEGRATION_PIXELSUMINTEGRATOR_H

#include <Eigen/Dense>

#include "MeanBackgroundIntegrator.h"

namespace nsx {

class DataSet;

//! \brief Peak integration using naive background estimation and subtraction.
class PixelSumIntegrator : public MeanBackgroundIntegrator {
public:
    //! Construct the pixel sum integrator
    /** \param fit_center update the peak center as part of integration
     *  \param fit_covariance update the peak shape covariance matrix as part of
     * integration
     */
    PixelSumIntegrator(bool fit_center, bool fit_covariance);
    ~PixelSumIntegrator();
    bool compute(sptrPeak3D peak, const IntegrationRegion& region) override;

private:
    bool _fitCenter;
    bool _fitCovariance;
};

} // end namespace nsx

#endif // CORE_INTEGRATION_PIXELSUMINTEGRATOR_H
