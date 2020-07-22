//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/integration/Profile1DIntegrator.h
//! @brief     Defines class Profile1DIntegrator
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef NSX_CORE_INTEGRATION_PROFILE1DINTEGRATOR_H
#define NSX_CORE_INTEGRATION_PROFILE1DINTEGRATOR_H

#include "core/shape/IPeakIntegrator.h"
#include "core/shape/ShapeLibrary.h"

namespace nsx {

//! Class to integrate peaks using 1d profile fitting.

class Profile1DIntegrator : public IPeakIntegrator {
 public:
    //! Construct integrator with given library, peak search radius, and frame bound
    Profile1DIntegrator();

    //! Compute the integrated intensity of the given peakd
    bool
    compute(Peak3D* peak, ShapeLibrary* shape_library, const IntegrationRegion& region) override;
};

} // namespace nsx

#endif // NSX_CORE_INTEGRATION_PROFILE1DINTEGRATOR_H
