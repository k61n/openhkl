#pragma once

#include <chrono>
#include <ctime>

#include "IStreamWrapper.h"

namespace nsx {

class AggregateStreamWrapper;

class Logger {
public:
    Logger(IStreamWrapper* wrapper);

    ~Logger();

    template<typename T>
    Logger& operator<<(T&& x);

private:

    IStreamWrapper* _wrapper;

    std::string _msg;
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



