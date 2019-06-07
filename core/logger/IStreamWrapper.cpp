//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/logger/IStreamWrapper.cpp
//! @brief     Defines ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "IStreamWrapper.h"

namespace nsx {

IStreamWrapper::IStreamWrapper(
    std::function<std::string()> prefix, std::function<std::string()> suffix)
    : _prefix(prefix), _suffix(suffix)
{
}

IStreamWrapper::~IStreamWrapper() {}

} // end namespace nsx
