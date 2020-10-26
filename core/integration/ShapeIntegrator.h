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

#ifndef NSX_CORE_INTEGRATION_SHAPEINTEGRATOR_H
#define NSX_CORE_INTEGRATION_SHAPEINTEGRATOR_H

#include "core/integration/PixelSumIntegrator.h"

namespace nsx {

class ShapeCollection;

//! Peak integrator class used to build a shape collection for profile fitting.

class ShapeIntegrator : public PixelSumIntegrator {
 public:
    //! Construct the integrator with the given shape collection, bounding box, and box shape.
    ShapeIntegrator(ShapeCollection* lib, const AABB& aabb, int nx, int ny, int nz);
    bool compute(
        Peak3D* peak, ShapeCollection* shape_collection, const IntegrationRegion& region) override;
    //! Returns the collection of cached peak shapes
    const ShapeCollection* collection() const;

 private:
    ShapeCollection* _collection;
    AABB _aabb;
    int _nx, _ny, _nz;
};

} // namespace nsx

#endif // NSX_CORE_INTEGRATION_SHAPEINTEGRATOR_H
