//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/gonio/AxisFactory.cpp
//! @brief     Implements ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include <stdexcept>
#include <string>

#include "core/gonio/AxisFactory.h"
#include "core/gonio/RotAxis.h"
#include "core/gonio/TransAxis.h"

namespace nsx {

AxisFactory::AxisFactory()
{
    registerCallback("rotation", &RotAxis::create);
    registerCallback("translation", &TransAxis::create);
}

AxisFactory::~AxisFactory() {}

} // end namespace nsx
