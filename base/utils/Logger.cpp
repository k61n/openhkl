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
    _ofs << std::boolalpha;
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

// Messenger definitions
Messenger::receiverHandle Messenger::addReceiver(Messenger::receiver_t rec_ptr)
{
    receiverHandle rec_h = NO_HANDLE;
    std::size_t i = 0;
    for (; i < MSG_RECEIVERS_MAXNR; ++i) {
        if (!_receivers[i]) {
            _receivers[i] = rec_ptr;
            break;
        }
    }

    if (i < MSG_RECEIVERS_MAXNR)
        rec_h = i;

    return rec_h;
}

//! Discard a receiver (if exists)
void Messenger::discardReceiver(const Messenger::receiverHandle rec_h) {
    if (rec_h >= 0 && rec_h < MSG_RECEIVERS_MAXNR)
        _receivers[rec_h] = nullptr;
}

void Messenger::send(const Message& msg) {
    for (std::size_t i = 0; i < MSG_RECEIVERS_MAXNR; ++i) {
        if (_receivers[i])
            _receivers[i](msg);
    }
}

} // namespace nsx
