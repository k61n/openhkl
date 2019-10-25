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

#ifndef CORE_INTEGRATION_PROFILE3DINTEGRATOR_H
#define CORE_INTEGRATION_PROFILE3DINTEGRATOR_H

#include "core/peak/IPeakIntegrator.h"
#include "core/shape/ShapeLibrary.h"

namespace nsx {

//! Peak integrator using 3d profile fitting, as described by Kabsch (1988, 2010).

class Profile3DIntegrator : public IPeakIntegrator {
 public:
    Profile3DIntegrator();
    bool compute(
        Peak3D* peak, ShapeLibrary* shape_library, 
        const IntegrationRegion& region) override;

};

} // namespace nsx

#endif // CORE_INTEGRATION_PROFILE3DINTEGRATOR_H
