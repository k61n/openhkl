#pragma once

#include <chrono>
#include <ctime>
#include <functional>
#include <sstream>

#include "IStreamWrapper.h"
#include "AggregateStreamWrapper.h"

namespace nsx {

class Logger {
public:
    Logger(IStreamWrapper* wrap, std::function<void(Logger&)> initialize = nullptr, std::function<void(Logger&)> finalize = nullptr);

    ~Logger();

    template<typename T>
    Logger& operator<<(T&& x);

private:

    IStreamWrapper* _wrap;

    std::string _msg;

    std::function<void(Logger&)> _initialize;

    std::function<void(Logger&)> _finalize;
};

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

    // If a finalize fuction has been provided use it to log in a suffix message
    if (_finalize) {
        _finalize(*this);
    }
    _wrap->print(_msg);
}

template<typename T>
Logger& Logger::operator<<(T&& x) {
    std::stringstream str;
    str << x;
    _msg += str.str();
    return *this;
}

auto current_time = []() -> std::string {

    auto now = std::chrono::system_clock::now();

    std::time_t tt = std::chrono::system_clock::to_time_t (now);
    std::string current_time = std::ctime(&tt);
    current_time.erase(current_time.size()-1);

    return current_time;
};

static AggregateStreamWrapper* g_wrapper = new AggregateStreamWrapper();

AggregateStreamWrapper* getWrapper()
{
    return g_wrapper;
}

auto debugLog() -> Logger
{
    auto initialize = [](Logger& log) {log << " -- DEBUG   " << current_time() << " -- ";};
    auto finalize = [](Logger& log) {log << "\n";};

    return Logger(g_wrapper, initialize, finalize);
};

auto infoLog() -> Logger
{
    auto initialize = [](Logger& log) {log << " -- INFO    " << current_time() << " -- ";};
    auto finalize = [](Logger& log) {log << "\n";};

    return Logger(g_wrapper, initialize, finalize);
};

auto warningLog() -> Logger
{
    auto initialize = [](Logger& log) {log << " -- WARNING " << current_time() << " -- ";};
    auto finalize = [](Logger& log) {log << "\n";};

    return Logger(g_wrapper, initialize, finalize);
};

auto errorLog() -> Logger
{
    auto initialize = [](Logger& log) {log << " -- ERROR   " << current_time() << " -- ";};
    auto finalize = [](Logger& log) {log << "\n";};

    return Logger(g_wrapper, initialize, finalize);
};

static std::function<Logger()> debug   = debugLog;
static std::function<Logger()> info    = infoLog;
static std::function<Logger()> warning = warningLog;
static std::function<Logger()> error   = errorLog;

} // end namespace nsx



