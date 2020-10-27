//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/integration/MeanBackgroundIntegrator.h
//! @brief     Defines class MeanBackgroundIntegrator
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef NSX_CORE_INTEGRATION_MEANBACKGROUNDINTEGRATOR_H
#define NSX_CORE_INTEGRATION_MEANBACKGROUNDINTEGRATOR_H

#include "core/shape/IPeakIntegrator.h"

namespace nsx {

//! Compute the mean background near a peak.

class MeanBackgroundIntegrator : public IPeakIntegrator {
 public:
    MeanBackgroundIntegrator();
    bool compute(
        Peak3D*, ShapeCollection* shape_collection, const IntegrationRegion& region) override;
};

} // namespace nsx

#endif // NSX_CORE_INTEGRATION_MEANBACKGROUNDINTEGRATOR_H
