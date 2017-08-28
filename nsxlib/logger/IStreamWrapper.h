#pragma once

#include <string>

namespace nsx {

class IStreamWrapper {
public:

    //! Constructor
    IStreamWrapper();

    //! Destructor
    virtual ~IStreamWrapper();

    //! Write a message to the stream
    virtual void print(const std::string& message)=0;
};

} // end namespace nsx
