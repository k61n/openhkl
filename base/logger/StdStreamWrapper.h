//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      base/logger/StdStreamWrapper.h
//! @brief     Defines class StdStreamWrapper
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef CORE_LOGGER_STDSTREAMWRAPPER_H
#define CORE_LOGGER_STDSTREAMWRAPPER_H

#include <ostream>

#include "base/logger/IStreamWrapper.h"

namespace nsx {

class StdStreamWrapper : public IStreamWrapper {
public:
    //! Constructor
    StdStreamWrapper(
        std::ostream& stream, std::function<std::string()> prefix = nullptr,
        std::function<std::string()> suffix = nullptr);
    virtual ~StdStreamWrapper();

    //! Write a message to the stream
    virtual void print(const std::string& message) override;

    virtual void printPrefix() override;

    virtual void printSuffix() override;

private:
    std::ostream& _stream;
};

} // namespace nsx

#endif // CORE_LOGGER_STDSTREAMWRAPPER_H
