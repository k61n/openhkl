#pragma once

#include <vector>

#include "IStreamWrapper.h"

namespace nsx {

class AggregateStreamWrapper : public IStreamWrapper {
public:

    AggregateStreamWrapper();

    virtual ~AggregateStreamWrapper();

    void addWrapper(IStreamWrapper* wrapper);

    virtual void print(const std::string& message) override;

private:

    std::vector<IStreamWrapper*> _wrappers;

};

} /* namespace nsx */
