#pragma once

#include <fstream>
#include <string>

#include "IStreamWrapper.h"

namespace nsx {

class LogFileStreamWrapper : public IStreamWrapper {
public:

    //! Constructor
    LogFileStreamWrapper(const std::string& logfile);

    //! Destructor
    virtual ~LogFileStreamWrapper();

    //! Write a message to the stream
    virtual void print(const std::string& message) override;

private:

    std::ofstream _stream;
};

} // end namespace nsx
