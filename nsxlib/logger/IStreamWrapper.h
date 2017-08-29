#pragma once

#include <string>

namespace nsx {

class IStreamWrapper {
public:

    //! Constructor
    IStreamWrapper();

    IStreamWrapper(const IStreamWrapper& other)=delete;

    //! Destructor
    virtual ~IStreamWrapper()=0;

    IStreamWrapper& operator=(const IStreamWrapper& oter)=delete;

    //! Write a message to the stream
    virtual void print(const std::string& message)=0;
};

} // end namespace nsx
