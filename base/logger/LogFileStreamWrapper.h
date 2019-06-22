//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      base/logger/LogFileStreamWrapper.h
//! @brief     Defines class LogFileStreamWrapper
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef BASE_LOGGER_LOGFILESTREAMWRAPPER_H
#define BASE_LOGGER_LOGFILESTREAMWRAPPER_H

#include "base/logger/IStreamWrapper.h"
#include <fstream>

namespace nsx {

class LogFileStreamWrapper : public IStreamWrapper {
 public:
    //! Constructor
    LogFileStreamWrapper(
        const std::string& logfile, std::function<std::string()> prefix = nullptr,
        std::function<std::string()> suffix = nullptr);
    virtual ~LogFileStreamWrapper();

    //! Write a message to the stream
    virtual void print(const std::string& message) override;

    virtual void printPrefix() override;

    virtual void printSuffix() override;

 private:
    std::ofstream _stream;
};

} // namespace nsx

#endif // BASE_LOGGER_LOGFILESTREAMWRAPPER_H
