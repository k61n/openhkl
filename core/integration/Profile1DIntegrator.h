//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/integration/Profile1DIntegrator.h
//! @brief     Defines ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef CORE_INTEGRATION_PROFILE1DINTEGRATOR_H
#define CORE_INTEGRATION_PROFILE1DINTEGRATOR_H

//@ #include <Eigen/Dense>

//@ #include "core/integration/ShapeLibrary.h"
#include "core/peak/IPeakIntegrator.h"

namespace nsx {

//@ class DataSet;

//! \brief Class to integrate peaks using 1d profile fitting.
class Profile1DIntegrator : public IPeakIntegrator {
public:
    //! Construct integrator with given library, peak search radius, and frame
    //! bound
    Profile1DIntegrator(sptrShapeLibrary library, double radius, double nframes);
    //! Compute the integrated intensity of the given peakd
    bool compute(sptrPeak3D peak, const IntegrationRegion& region) override;

private:
    sptrShapeLibrary _library;
    double _radius;
    double _nframes;
};

} // namespace nsx

#endif // CORE_INTEGRATION_PROFILE1DINTEGRATOR_H
