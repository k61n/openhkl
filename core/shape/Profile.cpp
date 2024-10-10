//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/shape/Profile.cpp
//! @brief     Implements class Profile
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "core/shape/Profile.h"

namespace ohkl {


Profile::Profile(Profile3D&& profile3d, Profile1D&& profile1d)
{
    _profile3d = std::move(profile3d);
    _profile1d = std::move(profile1d);
}


} // namespace ohkl
