//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/integration/ShapeIntegrator.h
//! @brief     Defines class ShapeIntegrator
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef CORE_INTEGRATION_SHAPEINTEGRATOR_H
#define CORE_INTEGRATION_SHAPEINTEGRATOR_H

#include "core/integration/PixelSumIntegrator.h"
#include "core/shape/ShapeLibrary.h"

namespace nsx {

//! Peak integrator class used to build a shape library for profile fitting.

class ShapeIntegrator : public PixelSumIntegrator {
 public:
    //! Construct the integrator with the given shape library, bounding box, and box shape.
    ShapeIntegrator(ShapeLibrary* lib, const AABB& aabb, int nx, int ny, int nz);
    bool
    compute(Peak3D* peak, ShapeLibrary* shape_library, const IntegrationRegion& region) override;
    //! Returns the library of cached peak shapes
    ShapeLibrary* library() const;

 private:
    ShapeLibrary* _library;
    AABB _aabb;
    int _nx, _ny, _nz;
};

} // namespace nsx

#endif // CORE_INTEGRATION_SHAPEINTEGRATOR_H
