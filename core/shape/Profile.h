//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/shape/Profile.h
//! @brief     Defines class Profile
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_CORE_SHAPE_PROFILE_H
#define OHKL_CORE_SHAPE_PROFILE_H

#include "core/shape/Profile1D.h"
#include "core/shape/Profile3D.h"

#include <optional>

namespace ohkl {

/*! \addtogroup python_api
 *  @{*/

/*! \brief Container for a Profile3D and a Profile1D
 *
 *  A profile is the average of many peaks. This class allows profiles to be
 *  summed and normalised for use in integration.
 */
class Profile {
 public:
    Profile(Profile3D&& profile3d, Profile1D&& profile1d);

    Profile3D* profile3d() { return &_profile3d; };
    Profile1D* profile1d() { return &_profile1d; };

 private:
    Profile3D _profile3d;
    Profile1D _profile1d;
};

/*! @}*/
} // namespace ohkl

#endif // OHKL_CORE_SHAPE_PROFILE_H
