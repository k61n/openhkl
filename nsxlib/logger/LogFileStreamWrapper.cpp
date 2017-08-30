#include "LogFileStreamWrapper.h"

namespace nsx {

LogFileStreamWrapper::LogFileStreamWrapper(const std::string& logfile) : IStreamWrapper(), _stream(logfile,std::fstream::app)
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

} // end namespace nsx
