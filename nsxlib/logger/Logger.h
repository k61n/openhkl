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

AggregateStreamWrapper* wrapper();

extern std::function<Logger()> debug;
extern std::function<Logger()> info;
extern std::function<Logger()> warning;
extern std::function<Logger()> error;

} // end namespace nsx



