#pragma once

#include <chrono>
#include <ctime>

#include "IStreamWrapper.h"

namespace nsx {

class AggregateStreamWrapper;

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

template<typename T>
Logger& Logger::operator<<(T&& x) {
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

} // end namespace nsx



