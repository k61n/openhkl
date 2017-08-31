#include "StdStreamWrapper.h"

namespace nsx {

StdStreamWrapper::StdStreamWrapper(std::ostream& stream, std::function<std::string()> prefix, std::function<std::string()> suffix)
: IStreamWrapper(prefix, suffix),
  _stream(stream)
{
}

StdStreamWrapper::~StdStreamWrapper() {
}

void StdStreamWrapper::print(const std::string& message)
{
    _stream << message;
}

void StdStreamWrapper::printPrefix()
{
    if (_prefix) {
        _stream<<_prefix();
    }
}

void StdStreamWrapper::printSuffix()
{
    if (_suffix) {
        _stream << _suffix();
    }
    _stream.flush();
}

} // end namespace nsx
