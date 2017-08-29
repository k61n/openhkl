#pragma once

#include <nsxlib/logger/IStreamWrapper.h>

class QtStreamWrapper : public nsx::IStreamWrapper {
public:

    //! Constructor
    QtStreamWrapper();

    //! Destructor
    virtual ~QtStreamWrapper();

    //! Write a message to the stream
    virtual void print(const std::string& message) override;
};
