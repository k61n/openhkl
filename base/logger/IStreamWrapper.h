//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/logger/IStreamWrapper.h
//! @brief     Defines class IStreamWrapper
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef CORE_LOGGER_ISTREAMWRAPPER_H
#define CORE_LOGGER_ISTREAMWRAPPER_H

#include <functional>

namespace nsx {

class IStreamWrapper {
public:
    //! Constructor
    IStreamWrapper(
        std::function<std::string()> prefix = nullptr,
        std::function<std::string()> suffix = nullptr);

    IStreamWrapper(const IStreamWrapper& other) = delete;
    virtual ~IStreamWrapper() = 0;

    IStreamWrapper& operator=(const IStreamWrapper& oter) = delete;

    //! Write a message to the stream
    virtual void print(const std::string& message) = 0;

    virtual void printPrefix() = 0;

    virtual void printSuffix() = 0;

protected:
    std::function<std::string()> _prefix;

    std::function<std::string()> _suffix;
};

} // namespace nsx

#endif // CORE_LOGGER_ISTREAMWRAPPER_H
