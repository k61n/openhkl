//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/integration/ISigmaIntegrator.h
//! @brief     Defines class ISigmaIntegrator
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_CORE_INTEGRATION_ISIGMAINTEGRATOR_H
#define OHKL_CORE_INTEGRATION_ISIGMAINTEGRATOR_H

#include "core/integration/PixelSumIntegrator.h"

namespace ohkl {

class Profile;

/*! \addtogroup python_api
 *  @{*/

/*! \brief Compute the integrated intensity via the I/sigma method used in RETREAT.
 *
 *  Described in E. Prince et al., J. Appl. Cryst., 30:133, 1997. doi:10.1107/S0021889896012824.
 *
 *  We take the numeric minimum,
 *  \f[
 *    \textrm{argmin}\left( \frac{\sigma^2(p)}{p^2} + \frac{\sigma^2(I)}{I^2} \right),
 *  \f]
 *  Which determines the end of the peak region, and use this to do profile
 *  fitting integration.
 *
 */

class ISigmaIntegrator : public PixelSumIntegrator {
 public:
    ISigmaIntegrator();

 protected:
    ComputeResult compute(
        Peak3D* peak, Profile* Profile, const IntegrationRegion& region) override;
};

/*! @}*/
} // namespace ohkl

#endif // OHKL_CORE_INTEGRATION_ISIGMAINTEGRATOR_H
