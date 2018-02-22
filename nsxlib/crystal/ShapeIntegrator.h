#pragma once

#include <Eigen/Dense>

#include "CrystalTypes.h"
#include "GeometryTypes.h"
#include "StrongPeakIntegrator.h"
#include "ShapeLibrary.h"

namespace nsx {

class DataSet;

class ShapeIntegrator: public StrongPeakIntegrator {
public:
    ShapeIntegrator(const AABB& aabb, int nx, int ny, int nz);
    bool compute(sptrPeak3D peak, const IntegrationRegion& region) override;

private:
    sptrShapeLibrary _library;
    AABB _aabb;
    int _nx, _ny, _nz;
};

} // end namespace nsx
