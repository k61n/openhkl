//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/integration/ShapeIntegrator.h
//! @brief     Defines ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef CORE_INTEGRATION_SHAPEINTEGRATOR_H
#define CORE_INTEGRATION_SHAPEINTEGRATOR_H

#include <Eigen/Dense>

#include "core/integration/PixelSumIntegrator.h"
#include "core/integration/ShapeLibrary.h"

namespace nsx {

class DataSet;

//! \brief Peak integrator class used to build a shape library for profile
//! fitting.
class ShapeIntegrator : public PixelSumIntegrator {
public:
    //! Construct the integrator with the given shape library, bounding box, and
    //! box shape.
    ShapeIntegrator(sptrShapeLibrary lib, const AABB& aabb, int nx, int ny, int nz);
    bool compute(sptrPeak3D peak, const IntegrationRegion& region) override;
    //! Returns the library of cached peak shapes
    sptrShapeLibrary library() const;

private:
    sptrShapeLibrary _library;
    AABB _aabb;
    int _nx, _ny, _nz;
};

} // end namespace nsx

#endif // CORE_INTEGRATION_SHAPEINTEGRATOR_H
