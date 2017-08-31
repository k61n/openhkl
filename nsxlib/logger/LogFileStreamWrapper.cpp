#include "LogFileStreamWrapper.h"

namespace nsx {

LogFileStreamWrapper::LogFileStreamWrapper(const std::string& logfile, std::function<std::string()> prefix, std::function<std::string()> suffix)
: IStreamWrapper(prefix,suffix),
  _stream(logfile,std::fstream::app)
{
}

LogFileStreamWrapper::~LogFileStreamWrapper()
{
    _stream.close();
}

void LogFileStreamWrapper::print(const std::string& message)
{
    _stream << message;
    _stream.flush();
}

void LogFileStreamWrapper::printPrefix()
{
    if (_prefix) {
        _stream<<_prefix();
    }
}

void LogFileStreamWrapper::printSuffix()
{
    if (_suffix) {
        _stream << _suffix();
    }
    _stream.flush();
}

} // end namespace nsx
