#include "IStreamWrapper.h"

namespace nsx {

IStreamWrapper::IStreamWrapper(std::function<std::string()> prefix, std::function<std::string()> suffix) : _prefix(prefix), _suffix(suffix) {
}

IStreamWrapper::~IStreamWrapper() {
}

} // end namespace nsx
