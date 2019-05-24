#pragma once

#include <Eigen/Dense>

#include "GeometryTypes.h"
#include "IPeakIntegrator.h"
#include "MeanBackgroundIntegrator.h"
#include "PixelSumIntegrator.h"
#include "ShapeLibrary.h"

namespace nsx {

class DataSet;

//! \brief Compute the integrated intensity via the I/sigma method used in
//! RETREAT.
class ISigmaIntegrator : public PixelSumIntegrator {
public:
    ISigmaIntegrator(sptrShapeLibrary library, double radius, double nframes);
    bool compute(sptrPeak3D peak, const IntegrationRegion& region) override;

private:
    sptrShapeLibrary _library;
    double _radius;
    double _nframes;
};

} // end namespace nsx
