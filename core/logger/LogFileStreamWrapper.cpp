//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/logger/LogFileStreamWrapper.cpp
//! @brief     Implements ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "core/logger/LogFileStreamWrapper.h"

namespace nsx {

LogFileStreamWrapper::LogFileStreamWrapper(
    const std::string& logfile, std::function<std::string()> prefix,
    std::function<std::string()> suffix)
    : IStreamWrapper(prefix, suffix), _stream(logfile, std::fstream::app)
{
}

LogFileStreamWrapper::~LogFileStreamWrapper()
{
    _stream.close();
}

void LogFileStreamWrapper::print(const std::string& message)
{
    _stream << message;
    _stream.flush();
}

void LogFileStreamWrapper::printPrefix()
{
    if (_prefix) {
        _stream << _prefix();
    }
}

void LogFileStreamWrapper::printSuffix()
{
    if (_suffix) {
        _stream << _suffix();
    }
    _stream.flush();
}

} // namespace nsx
