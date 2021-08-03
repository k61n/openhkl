//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      base/utils/Logger.h
//! @brief     Defines class Logger
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef NSX_BASE_UTILS_LOGGER_H
#define NSX_BASE_UTILS_LOGGER_H

#include <fstream>
#include <string>

// usage: DBG("This is my debug message nr.", 1);
#define DBG(...)                                                                                   \
    nsx::nsxlog(                                                                                   \
        nsx::Level::Info, "DBG> ", __FILE__, ":L", __LINE__, ":", __FUNCTION__, ": ", __VA_ARGS__)


namespace nsx {

//! Verbosity of the logger
enum class Level {
    Off = 0,
    Info,
    Error,
    Warning,
    Debug,
};

struct Message {
    Level level;
    std::string sender;
    std::string header;
    std::string body;
};

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
            _ofs << time() << " " << static_cast<int>(verbosity) << " ";
            ((_ofs << messages), ...) << std::endl; // unpack messages separated by spaces
        }
    }
    void start(const std::string& filename, const Level& min_level); //! initialise

 private:
    static Logger* m_logger; //! The single instance

    Logger() = default;
    ~Logger() = default;
    Logger(const Logger&) = delete;

    std::string time() const; //! get the time as a string

    Level _max_print_level = Level::Warning; //! print level (default Warning)
    std::ofstream _ofs;
};

//! Global logger function (prefixed with "nsx" to facilitate grepping)
//! Usage: nsxlog(Level::Warning, "your message", 1, 3.14, "test")
template <typename... T> inline void nsxlog(const Level& level, const T&... messages)
{
    Logger::instance().log(level, messages...);
}

} // namespace nsx

#endif // NSX_BASE_UTILS_LOGGER_H
