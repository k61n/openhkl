//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/integration/ShapeIntegrator.h
//! @brief     Defines class ShapeIntegrator
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_CORE_INTEGRATION_SHAPEINTEGRATOR_H
#define OHKL_CORE_INTEGRATION_SHAPEINTEGRATOR_H

#include "core/integration/PixelSumIntegrator.h"

namespace ohkl {

class ShapeModel;

/*! \addtogroup python_api
 *  @{*/

/*! \brief Integrate a peak to generate a profile for ShapeModel*/
class ShapeIntegrator : public PixelSumIntegrator {
 public:
    //! Construct the integrator with the given shape collection, bounding box, and box shape.
    ShapeIntegrator(ShapeModel* lib, const AABB& aabb, int nx, int ny, int nz);

 protected:
    //! Integrate a peak
    bool compute(Peak3D* peak, ShapeModel* shape_model, const IntegrationRegion& region) override;
    //! Returns the collection of cached peak shapes
    const ShapeModel* collection() const;

 private:
    ShapeModel* _collection;
    AABB _aabb;
    int _nx, _ny, _nz;
};

/*! @}*/
} // namespace ohkl

#endif // OHKL_CORE_INTEGRATION_SHAPEINTEGRATOR_H
