//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
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
#include <sstream>

namespace nsx {

Logger* Logger::m_logger = nullptr;

void Logger::start(const std::string& filename, const Level& level)
{
    _ofs.open(filename);
    _max_print_level = level;
}

Logger& Logger::instance()
{
    if (m_logger == nullptr)
        m_logger = new Logger();
    return *m_logger;
}

std::string Logger::time() const // yoinked from BornAgain
{
    using clock = std::chrono::system_clock;

    std::stringstream output;
    std::time_t current_time = clock::to_time_t(clock::now());
    output << std::put_time(std::gmtime(&current_time), "%Y-%b-%d %T");
    return output.str();
}

} // namespace nsx
