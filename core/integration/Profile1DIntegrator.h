#pragma once

#include <Eigen/Dense>

#include "IPeakIntegrator.h"
#include "ShapeLibrary.h"

namespace nsx {

class DataSet;

//! \brief Class to integrate peaks using 1d profile fitting.
class Profile1DIntegrator : public IPeakIntegrator {
public:
    //! Construct integrator with given library, peak search radius, and frame
    //! bound
    Profile1DIntegrator(sptrShapeLibrary library, double radius, double nframes);
    //! Compute the integrated intensity of the given peakd
    bool compute(sptrPeak3D peak, const IntegrationRegion& region) override;

private:
    sptrShapeLibrary _library;
    double _radius;
    double _nframes;
};

} // end namespace nsx
