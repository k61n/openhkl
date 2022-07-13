//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      base/utils/Logger.cpp
//! @brief     Defines class Logger
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "base/utils/Logger.h"

#include <chrono>
#include <iomanip>

namespace ohkl {

Logger* Logger::m_logger = nullptr;

void Logger::start(const std::string& filename, const Level& level)
{
    _ofs.open(filename);
    _max_print_level = level;
    _ofs << std::boolalpha;
}

Logger& Logger::instance()
{
    if (m_logger == nullptr)
        m_logger = new Logger();
    return *m_logger;
}

} // namespace ohkl
