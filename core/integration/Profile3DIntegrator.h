//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/integration/Profile3DIntegrator.h
//! @brief     Defines class Profile3DIntegrator
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef NSX_CORE_INTEGRATION_PROFILE3DINTEGRATOR_H
#define NSX_CORE_INTEGRATION_PROFILE3DINTEGRATOR_H

#include "core/shape/IPeakIntegrator.h"
#include "core/shape/ShapeCollection.h"

namespace nsx {

//! Peak integrator using 3d profile fitting, as described by Kabsch (1988, 2010).

class Profile3DIntegrator : public IPeakIntegrator {
 public:
    Profile3DIntegrator();
    bool compute(
        Peak3D* peak, ShapeCollection* shape_collection, const IntegrationRegion& region) override;
};

} // namespace nsx

#endif // NSX_CORE_INTEGRATION_PROFILE3DINTEGRATOR_H
