//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/logger/LogFileStreamWrapper.h
//! @brief     Defines ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef CORE_LOGGER_LOGFILESTREAMWRAPPER_H
#define CORE_LOGGER_LOGFILESTREAMWRAPPER_H

#include <fstream>
#include <string>

#include "core/logger/IStreamWrapper.h"

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

#endif // CORE_LOGGER_LOGFILESTREAMWRAPPER_H
