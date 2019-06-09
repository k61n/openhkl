//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/logger/StdStreamWrapper.cpp
//! @brief     Implements ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "core/logger/StdStreamWrapper.h"

namespace nsx {

StdStreamWrapper::StdStreamWrapper(
    std::ostream& stream, std::function<std::string()> prefix, std::function<std::string()> suffix)
    : IStreamWrapper(prefix, suffix), _stream(stream)
{
}

StdStreamWrapper::~StdStreamWrapper() {}

void StdStreamWrapper::print(const std::string& message)
{
    _stream << message;
}

void StdStreamWrapper::printPrefix()
{
    if (_prefix) {
        _stream << _prefix();
    }
}

void StdStreamWrapper::printSuffix()
{
    if (_suffix) {
        _stream << _suffix();
    }
    _stream.flush();
}

} // namespace nsx
