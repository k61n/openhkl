//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/logger/AggregateStreamWrapper.h
//! @brief     Defines class AggregateStreamWrapper
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef CORE_LOGGER_AGGREGATESTREAMWRAPPER_H
#define CORE_LOGGER_AGGREGATESTREAMWRAPPER_H

#include <vector>

#include "core/logger/IStreamWrapper.h"

namespace nsx {

class AggregateStreamWrapper : public IStreamWrapper {
public:
    AggregateStreamWrapper(
        std::function<std::string()> prefix = nullptr,
        std::function<std::string()> suffix = nullptr);

    virtual ~AggregateStreamWrapper();

    void addWrapper(IStreamWrapper* wrapper);

    virtual void print(const std::string& message) override;

    virtual void printPrefix() override;

    virtual void printSuffix() override;

private:
    std::vector<IStreamWrapper*> _wrappers;
};

} // namespace nsx

#endif // CORE_LOGGER_AGGREGATESTREAMWRAPPER_H
