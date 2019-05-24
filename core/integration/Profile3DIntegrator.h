#pragma once

#include <Eigen/Dense>

#include "IPeakIntegrator.h"
#include "ShapeLibrary.h"

namespace nsx {

class DataSet;

//! \brief Peak integrator using 3d profile fitting, as described by Kabsch
//! (1988, 2010).
class Profile3DIntegrator : public IPeakIntegrator {
public:
    Profile3DIntegrator(
        sptrShapeLibrary library, double radius, double nframes, bool detector_space);
    bool compute(sptrPeak3D peak, const IntegrationRegion& region) override;

private:
    sptrShapeLibrary _library;
    double _radius;
    double _nframes;
};

} // end namespace nsx
