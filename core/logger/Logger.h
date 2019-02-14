#pragma once

#include <functional>
#include <memory>
#include <sstream>

#include "IStreamWrapper.h"

namespace nsx {

class Logger {
public:
  // Note: necessary to delete this for MSVC
  Logger() = delete;
  Logger(IStreamWrapper *wrapper);
  // Note: necessary to delete this for MSVC build
  Logger(const Logger &other) = delete;
  Logger(Logger &&other) = default;

  ~Logger();

  template <typename T> Logger &operator<<(T &&x);

private:
  std::unique_ptr<IStreamWrapper> _wrapper;
  std::string _msg;
};

template <typename T> Logger &Logger::operator<<(T &&x) {
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
