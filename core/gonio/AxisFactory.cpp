//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/gonio/AxisFactory.cpp
//! @brief     Implements class AxisFactory
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include <stdexcept>
#include <string>

#include "core/gonio/AxisFactory.h"
#include "core/gonio/RotAxis.h"
#include "core/raw/DataKeys.h"

namespace ohkl {

AxisFactory::AxisFactory()
{
    registerCallback("rotation", &RotAxis::create);
}

} // namespace ohkl
