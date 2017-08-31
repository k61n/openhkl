#pragma once

#include <ostream>

#include "IStreamWrapper.h"

namespace nsx {

class StdStreamWrapper : public IStreamWrapper {
public:

    //! Constructor
    StdStreamWrapper(std::ostream& stream, std::function<std::string()> prefix=nullptr, std::function<std::string()> suffix=nullptr);

    //! Destructor
    virtual ~StdStreamWrapper();

    //! Write a message to the stream
    virtual void print(const std::string& message) override;

    virtual void printPrefix() override;

    virtual void printSuffix() override;

private:
    std::ostream& _stream;
};

} // end namespace nsx
