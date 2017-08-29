#pragma once

#include <ostream>

#include "IStreamWrapper.h"

namespace nsx {

class StdStreamWrapper : public IStreamWrapper {
public:

    //! Constructor
    StdStreamWrapper(std::ostream& stream);

    //! Destructor
    virtual ~StdStreamWrapper();

    //! Write a message to the stream
    virtual void print(const std::string& message) override;

private:
    std::ostream& _stream;
};

} // end namespace nsx
