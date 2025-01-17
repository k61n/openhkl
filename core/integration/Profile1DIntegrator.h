//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/integration/Profile1DIntegrator.h
//! @brief     Defines class Profile1DIntegrator
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_CORE_INTEGRATION_PROFILE1DINTEGRATOR_H
#define OHKL_CORE_INTEGRATION_PROFILE1DINTEGRATOR_H

#include "core/integration/IIntegrator.h"

namespace ohkl {

/*! \addtogroup python_api
 *  @{*/

/*! \brief Peak integrator using 1D profile fitting.
 *
 *  Described in:
 *  - W. Kabsch, J. Appl. Crystallography, 21:916, 1988. doi:10.1107/S0021889888007903
 *  - W. Kabsch, Acta Crystallographica D, 66:133, 2010. doi:10.1107/s0907444909047374
 *
 *  The implementation is identical to the Profile3DIntegrator, except the
 *  susbscripts \f$i\f$ refer to elements of a 1D profile instead of a 3D
 *  profile.
 */
class Profile1DIntegrator : public IIntegrator {
    friend class UnitTest_Profile1DIntegrator;
 public:
    //! Construct integrator
    Profile1DIntegrator();

 protected:
    //! Integrate a peak
    ComputeResult compute(
        Peak3D* peak, Profile* profile, const IntegrationRegion& region) override;
};

/*! @}*/
} // namespace ohkl

#endif // OHKL_CORE_INTEGRATION_PROFILE1DINTEGRATOR_H
