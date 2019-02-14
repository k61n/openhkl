#pragma once

#include <fstream>
#include <string>

#include "IStreamWrapper.h"

namespace nsx {

class LogFileStreamWrapper : public IStreamWrapper {
public:
  //! Constructor
  LogFileStreamWrapper(const std::string &logfile,
                       std::function<std::string()> prefix = nullptr,
                       std::function<std::string()> suffix = nullptr);

  //! Destructor
  virtual ~LogFileStreamWrapper();

  //! Write a message to the stream
  virtual void print(const std::string &message) override;

  virtual void printPrefix() override;

  virtual void printSuffix() override;

private:
  std::ofstream _stream;
};

} // end namespace nsx
