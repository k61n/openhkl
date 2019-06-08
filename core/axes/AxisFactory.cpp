//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/axes/AxisFactory.cpp
//! @brief     Defines ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include <stdexcept>
#include <string>

#include "core/axes/AxisFactory.h"
#include "core/axes/RotAxis.h"
#include "core/axes/TransAxis.h"

namespace nsx {

AxisFactory::AxisFactory()
{
    registerCallback("rotation", &RotAxis::create);
    registerCallback("translation", &TransAxis::create);
}

AxisFactory::~AxisFactory() {}

} // end namespace nsx
