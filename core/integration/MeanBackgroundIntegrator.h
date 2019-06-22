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

#ifndef CORE_INTEGRATION_MEANBACKGROUNDINTEGRATOR_H
#define CORE_INTEGRATION_MEANBACKGROUNDINTEGRATOR_H

#include "core/peak/IPeakIntegrator.h"

namespace nsx {

//! Compute the mean background near a peak.

class MeanBackgroundIntegrator : public IPeakIntegrator {
 public:
    MeanBackgroundIntegrator();
    bool compute(sptrPeak3D, const IntegrationRegion& region) override;
};

} // namespace nsx

#endif // CORE_INTEGRATION_MEANBACKGROUNDINTEGRATOR_H
