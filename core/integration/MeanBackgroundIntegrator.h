
#pragma once

#include <Eigen/Dense>

#include "IPeakIntegrator.h"

namespace nsx {

class DataSet;

//! \brief Compute the mean background near a peak.
class MeanBackgroundIntegrator: public IPeakIntegrator {
public:
    MeanBackgroundIntegrator();
    bool compute(sptrPeak3D peak, const IntegrationRegion& region) override;
};

} // end namespace nsx
