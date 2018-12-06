#pragma once

#include <Eigen/Dense>

#include "PixelSumIntegrator.h"
#include "ShapeLibrary.h"

namespace nsx {

class DataSet;

//! \brief Peak integrator class used to build a shape library for profile fitting.
class ShapeIntegrator: public PixelSumIntegrator {
public:
    //! Construct the integrator with the given shape library, bounding box, and box shape.
    ShapeIntegrator(sptrShapeLibrary lib, const AABB& aabb, int nx, int ny, int nz);
    bool compute(sptrPeak3D peak, const IntegrationRegion& region) override;
    //! Return the library of cached peak shapes
    sptrShapeLibrary library() const;

private:
    sptrShapeLibrary _library;
    AABB _aabb;
    int _nx, _ny, _nz;
};

} // end namespace nsx
