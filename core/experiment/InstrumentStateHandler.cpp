//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/experiment/InstrumentStateHandloer.cpp
//! @brief     Handles instrument state manipulations for Experiment object
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "core/experiment/InstrumentStateHandler.h"

#include "base/utils/Logger.h"
#include "core/data/DataSet.h"
#include "core/data/DataTypes.h"
#include "core/instrument/InstrumentState.h"
#include "core/instrument/InstrumentStateSet.h"

namespace nsx {

bool InstrumentStateHandler::addInstrumentStateSet(sptrDataSet data)
{
    if (hasInstrumentStateSet(data)) return false;
    nsxlog(Level::Info, "InstrumentStateHandler::addInstrumentStateSet for DataSet'", data->name());
    std::unique_ptr<InstrumentStateSet> ptr = std::make_unique<InstrumentStateSet>(data);
    ptr->setId(_last_index++);
    _instrumentstate_map.insert({data, std::move(ptr)});
    return hasInstrumentStateSet(data);
}

bool InstrumentStateHandler::addInstrumentStateSet(
    sptrDataSet data, const InstrumentStateList& states)
{
    if (hasInstrumentStateSet(data)) return false;
    nsxlog(Level::Info, "InstrumentStateHandler::addInstrumentStateSet for DataSet'", data->name());
    std::unique_ptr<InstrumentStateSet> ptr = std::make_unique<InstrumentStateSet>(data, states);
    ptr->setId(_last_index++);
    _instrumentstate_map.insert({data, std::move(ptr)});
    return hasInstrumentStateSet(data);
}

bool InstrumentStateHandler::hasInstrumentStateSet(const sptrDataSet& data) const
{
    auto states = _instrumentstate_map.find(data);
    return (states != _instrumentstate_map.end());
}

InstrumentStateSet* InstrumentStateHandler::getInstrumentStateSet(const sptrDataSet& data)
{
    if (hasInstrumentStateSet(data)) return _instrumentstate_map[data].get();
    return nullptr;
}

void InstrumentStateHandler::removeInstrumentStateSet(const sptrDataSet& data)
{
    std::map<sptrDataSet, std::unique_ptr<InstrumentStateSet>>::iterator it;
    for (it = _instrumentstate_map.begin(); it != _instrumentstate_map.end(); ++it)
        if (it->first == data) _instrumentstate_map.erase(it);
}

} // namespace nsx
