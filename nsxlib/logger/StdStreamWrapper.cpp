#include "StdStreamWrapper.h"

namespace nsx {

StdStreamWrapper::StdStreamWrapper(std::ostream& stream) : _stream(stream), IStreamWrapper() {
}

StdStreamWrapper::~StdStreamWrapper() {
}

void StdStreamWrapper::print(const std::string& message)
{
    _stream << message;
    _stream.flush();
}

} // end namespace nsx

