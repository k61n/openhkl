#include <algorithm>

#include "AggregateStreamWrapper.h"

namespace nsx {

AggregateStreamWrapper::AggregateStreamWrapper(std::function<std::string()> prefix, std::function<std::string()> suffix)
: IStreamWrapper(prefix,suffix),
  _wrappers()
{
}

AggregateStreamWrapper::~AggregateStreamWrapper()
{
    for (auto w : _wrappers)
        delete w;
    _wrappers.clear();
}

void AggregateStreamWrapper::print(const std::string& message)
{
    for (auto w : _wrappers) {
        w->print(message);
    }
}

void AggregateStreamWrapper::addWrapper(IStreamWrapper* wrapper)
{
    auto it = std::find(_wrappers.begin(),_wrappers.end(),wrapper);

    if (it == _wrappers.end()) {
        _wrappers.emplace_back(wrapper);
    }
}

void AggregateStreamWrapper::printPrefix()
{
    for (auto w : _wrappers) {
        w->printPrefix();
    }
}

void AggregateStreamWrapper::printSuffix()
{
    for (auto w : _wrappers) {
        w->printSuffix();
    }
}

} // end namespace nsx
