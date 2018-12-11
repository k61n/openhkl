#pragma once

#include <functional>
#include <vector>

#include "IStreamWrapper.h"

namespace nsx {

class AggregateStreamWrapper : public IStreamWrapper {
public:
    AggregateStreamWrapper(
        std::function<std::string()> prefix = nullptr,
        std::function<std::string()> suffix = nullptr);

    virtual ~AggregateStreamWrapper();

    void addWrapper(IStreamWrapper* wrapper);

    virtual void print(const std::string& message) override;

    virtual void printPrefix() override;

    virtual void printSuffix() override;

private:
    std::vector<IStreamWrapper*> _wrappers;
};

} // end namespace nsx
