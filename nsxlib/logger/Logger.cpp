#include "Logger.h"

#include <chrono>
#include <ctime>

#include "AggregateStreamWrapper.h"

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

static AggregateStreamWrapper* g_wrapper = new AggregateStreamWrapper();

AggregateStreamWrapper* wrapper()
{
    return g_wrapper;
}

auto current_time = []() -> std::string
{
    auto now = std::chrono::system_clock::now();

    std::time_t tt = std::chrono::system_clock::to_time_t (now);
    std::string current_time = std::ctime(&tt);
    current_time.erase(current_time.size()-1);

    return current_time;
};

auto debugLog() -> Logger
{
    auto initialize = [](Logger& log) {log << " -- DEBUG   " << current_time() << " -- ";};
    auto finalize = [](Logger& log) {log << "\n";};

    return Logger(g_wrapper, initialize, finalize);
};
std::function<Logger()> debug = debugLog;

auto infoLog() -> Logger
{
    auto initialize = [](Logger& log) {log << " -- INFO    " << current_time() << " -- ";};
    auto finalize = [](Logger& log) {log << "\n";};

    return Logger(g_wrapper, initialize, finalize);
};
std::function<Logger()> info = infoLog;

auto warningLog() -> Logger
{
    auto initialize = [](Logger& log) {log << " -- WARNING " << current_time() << " -- ";};
    auto finalize = [](Logger& log) {log << "\n";};

    return Logger(g_wrapper, initialize, finalize);
};
std::function<Logger()> warning = warningLog;

auto errorLog() -> Logger
{
    auto initialize = [](Logger& log) {log << " -- ERROR   " << current_time() << " -- ";};
    auto finalize = [](Logger& log) {log << "\n";};

    return Logger(g_wrapper, initialize, finalize);
};
std::function<Logger()> error = errorLog;

} // end namespace nsx



