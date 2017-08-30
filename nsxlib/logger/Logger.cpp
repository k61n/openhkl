#include "Logger.h"

#include <chrono>
#include <ctime>
#include <fstream>

#include "AggregateStreamWrapper.h"
#include "LogFileStreamWrapper.h"

namespace nsx {

Logger::Logger(IStreamWrapper* wrap, std::function<void(Logger&)> initialize, std::function<void(Logger&)> finalize)
: _wrap(wrap),
  _msg(),
  _initialize(initialize),
  _finalize(finalize)
{
    // If an initialize fuction has been provided use it to log in a prefix message
    if (_initialize) {
        _initialize(*this);
    }
}

Logger::~Logger(){
    if (_finalize) {
        _finalize(*this);
    }
    _wrap->print(_msg);
}

auto current_time() -> std::string
{
    auto now = std::chrono::system_clock::now();

    std::time_t tt = std::chrono::system_clock::to_time_t (now);
    std::string current_time = std::ctime(&tt);
    current_time.erase(current_time.size()-1);

    return current_time;
};

std::string currentTime()
{
    return current_time();
}

auto debug_log() -> Logger
{
    auto initialize = [](Logger& log) {log << "DEBUG" << current_time() << "--";};
    auto finalize = [](Logger& log) {log << "\n";};

    AggregateStreamWrapper* wrapper = new AggregateStreamWrapper();
    wrapper->addWrapper(new LogFileStreamWrapper("nsx_debug.txt"));

    return Logger(wrapper, initialize, finalize);
};

auto info_log() -> Logger
{
    auto initialize = [](Logger& log) {log << "INFO " << current_time() << "--";};
    auto finalize = [](Logger& log) {log << "\n";};

    AggregateStreamWrapper* wrapper = new AggregateStreamWrapper();
    wrapper->addWrapper(new LogFileStreamWrapper("nsx_info.txt"));

    return Logger(wrapper, initialize, finalize);
};

auto error_log() -> Logger
{
    auto initialize = [](Logger& log) {log << "ERROR" << current_time() << "--";};
    auto finalize = [](Logger& log) {log << "\n";};

    AggregateStreamWrapper* wrapper = new AggregateStreamWrapper();
    wrapper->addWrapper(new LogFileStreamWrapper("nsx_error.txt"));

    return Logger(wrapper, initialize, finalize);
};

static std::function<Logger()> g_debug = debug_log;
static std::function<Logger()> g_info = info_log;
static std::function<Logger()> g_error = error_log;

Logger debug()
{
    return g_debug();
}

void setDebug(std::function<Logger()> debug)
{
    g_debug = debug;
}

Logger info()
{
    return g_info();
}

void setInfo(std::function<Logger()> info)
{
    g_info = info;
}

Logger error()
{
    return g_error();
}

void setError(std::function<Logger()> error)
{
    g_error = error;
}

} // end namespace nsx



