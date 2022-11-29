//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      base/utils/Logger.h
//! @brief     Defines class Logger
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_BASE_UTILS_LOGGER_H
#define OHKL_BASE_UTILS_LOGGER_H

#include "base/utils/LogLevel.h"
#include "base/utils/LogMessenger.h"
#include "base/utils/StringIO.h" // datetime_str
#include <fstream> // ofstream
#include <sstream> // stringstream
#include <string>

// usage: DBG("This is my debug message nr.", 1);
#define DBG(...)                                                                                   \
    ohkl::ohklLog(                                                                                 \
        ohkl::Level::Info, "DBG> ", __FILE__, ":L", __LINE__, ":", __FUNCTION__, ": ",             \
        __VA_ARGS__)


namespace ohkl {

//! A singleton class for logging
class Logger {

 public:
    static Logger& instance(); //! call the single instance
    //! Add timestamped messages to log. Variadic function takes int or Level as first
    //! argument (the print level), followed by a list of messages of arbitrary type that
    //! can be added to an ofstream (the messages)
    template <typename... T> void log(const Level& verbosity, const T&... messages)
    {
        if (verbosity <= _max_print_level) {
            _ofs << datetime_str() << " " << static_cast<int>(verbosity) << " ";
            ((_ofs << messages), ...) << std::endl; // unpack messages
        }
    }
    void start(const std::string& filename, const Level& min_level); //! initialise

    //! Log messenger
    LogMessenger Msg;

 private:
    static Logger* m_logger; //! The single instance

    Logger() = default;
    ~Logger() = default;
    Logger(const Logger&) = delete;

    Level _max_print_level = Level::Warning; //! print level (default Warning)
    std::ofstream _ofs;
};

//! Global logger function (prefixed with "ohkl" to facilitate grepping)
//! Usage: ohklLog(Level::Warning, "your message", 1, 3.14, "test")
template <typename... T> inline void ohklLog(const Level& level, const T&... messages)
{
    Logger::instance().log(level, messages...);
    ohklMsg(level, messages...);
}

//! Global messaging function (prefixed with "ohkl" to facilitate grepping)
//! Usage: ohklMsg(Level::Warning, "your message", 1, 3.14, "test")
template <typename... T> inline void ohklMsg(const Level& level, const T&... messages)
{
    // Prepare a log message
    std::stringstream ss;
    ((ss << messages), ...); // unpack messages
    LogMessage msg{
        level,
        /* header */ datetime_str(),
        /* body */ ss.str()};
    Logger::instance().Msg.send(msg);
}

} // namespace ohkl

#endif // OHKL_BASE_UTILS_LOGGER_H
