//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      base/logger/Logger.h
//! @brief     Defines class Logger
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef BASE_LOGGER_LOGGER_H
#define BASE_LOGGER_LOGGER_H

#include <iostream>
#include <memory>
#include <sstream>

#include "base/logger/IStreamWrapper.h"

namespace nsx {

class Logger {
 public:
    // Note: necessary to delete this for MSVC
    Logger() = delete;
    Logger(IStreamWrapper* wrapper);
    // Note: necessary to delete this for MSVC build
    Logger(const Logger& other) = delete;
    Logger(Logger&& other) = default;

    ~Logger();

    template <typename T> Logger& operator<<(T&& x);

 private:
    std::unique_ptr<IStreamWrapper> _wrapper;
    std::string _msg;
};

template <typename T> Logger& Logger::operator<<(T&& x)
{
    std::cout << x << std::endl;
    std::stringstream str;
    str << " " << x;
    _msg += str.str();
    return *this;
}

std::string currentTime();

Logger debug();
void setDebug(std::function<Logger()> debug);

Logger info();
void setInfo(std::function<Logger()> info);

Logger error();
void setError(std::function<Logger()> error);

} // namespace nsx

#endif // BASE_LOGGER_LOGGER_H
