#include "Logger.h"

#include <chrono>
#include <ctime>
#include <fstream>

#include "AggregateStreamWrapper.h"
#include "LogFileStreamWrapper.h"

namespace nsx {

Logger::Logger(IStreamWrapper* wrapper)
: _wrapper(wrapper),
  _msg()
{
    _wrapper->printPrefix();
}

Logger::~Logger(){
    _wrapper->print(_msg);
    _wrapper->printSuffix();
    delete _wrapper;
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
    auto initialize = []() -> std::string {return "[DEBUG] " + current_time();};
    auto finalize = []() -> std::string {return "\n";};

    AggregateStreamWrapper* wrapper = new AggregateStreamWrapper();
    wrapper->addWrapper(new LogFileStreamWrapper("nsx_debug.txt",initialize,finalize));

    return Logger(wrapper);
};

auto info_log() -> Logger
{
    auto initialize = []() -> std::string {return "[INFO]  " + current_time();};
    auto finalize = []() -> std::string {return "\n";};

    AggregateStreamWrapper* wrapper = new AggregateStreamWrapper();
    wrapper->addWrapper(new LogFileStreamWrapper("nsx_info.txt",initialize,finalize));

    return Logger(wrapper);
};

auto error_log() -> Logger
{
    auto initialize = []() -> std::string {return "[ERROR] " + current_time();};
    auto finalize = []() -> std::string {return "\n";};

    AggregateStreamWrapper* wrapper = new AggregateStreamWrapper();
    wrapper->addWrapper(new LogFileStreamWrapper("nsx_error.txt",initialize,finalize));

    return Logger(wrapper);
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



