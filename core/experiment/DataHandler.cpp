//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/experiment/DataHandler.cpp
//! @brief     Handles DataSet manipulations for Experiment object
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "core/experiment/DataHandler.h"

#include "base/utils/Logger.h"
#include "core/data/DataSet.h"
#include "core/experiment/InstrumentStateHandler.h"
#include "core/instrument/Diffractometer.h"
#include "core/raw/DataKeys.h"

#include <iostream>
#include <stdexcept>

namespace nsx {

DataHandler::DataHandler(
    const std::string& experiment_name, const std::string& diffractometerName,
    InstrumentStateHandler* instrument_state_handler)
    : _experiment_name{experiment_name}
    , _instrument_state_handler(instrument_state_handler)
{
    if (diffractometerName == nsx::kw_diffractometerDefaultName) {
        nsxlog(
            Level::Warning,
            "DataHandler: Diffractometer is not set for the experiment '" + experiment_name + "'");
    } else {
        _diffractometer.reset(Diffractometer::create(diffractometerName));
    }
}

Diffractometer* DataHandler::getDiffractometer()
{
    return _diffractometer.get();
}

void DataHandler::setDiffractometer(const std::string& diffractometerName)
{
    _diffractometer.reset(Diffractometer::create(diffractometerName));
}

const DataMap* DataHandler::getDataMap() const
{
    return &_data_map;
}

DataList DataHandler::getAllData() const
{
    DataList numors;
    for (auto const& [key, val] : _data_map)
        numors.push_back(val);
    return numors;
}

sptrDataSet DataHandler::getData(std::string name) const
{
    auto it = _data_map.find(name);
    if (it == _data_map.end()) {
        throw std::runtime_error(
            "The data key " + name + " could not be found in the experiment " + _experiment_name);
    }
    return it->second;
}

bool DataHandler::addData(sptrDataSet data, std::string name)
{
    if (name.empty())
        name = data->name();

    if (name.empty())
        throw std::invalid_argument("DataHandler::addData: Data name cannot be empty");

    if (hasData(name)){
        throw std::invalid_argument("DataHandler::addData: Data name must be unique");
        return false;
    }

    // Add the data only if it does not exist in the current data map
    if (_data_map.find(name) != _data_map.end())
        throw std::invalid_argument(
            "DataHandler::addData: Data name '" + name + "' already exists.");

    const auto& metadata = data->metadata();

    const std::string diffName = metadata.key<std::string>(nsx::at_diffractometer);

    if (!(diffName.compare(_diffractometer->name()) == 0)) {
        throw std::runtime_error(
            "Mismatch between the diffractometer assigned to "
            "the experiment, '"
            + _diffractometer->name()
            + "', "
              "and the data, '"
            + diffName + "'");
    }
    const double wav = metadata.key<double>(nsx::at_wavelength);

    // ensure that there is at least one monochromator; if not, create a new one.
    if (_diffractometer->source().nMonochromators() == 0) {
        Monochromator mono(nsx::kw_monochromatorDefaultName);
        _diffractometer->source().addMonochromator(mono);
    }

    auto& mono = _diffractometer->source().selectedMonochromator();

    if (_data_map.empty())
        mono.setWavelength(wav);
    else {
        if (std::abs(wav - mono.wavelength()) > 1e-5)
            throw std::runtime_error("trying to mix data with different wavelengths");
    }

    nsxlog(
        Level::Info, "DataHandler::addData: adding DataSet '", name, "': ", data->nFrames(),
        " frames");
    _data_map.insert(std::make_pair(name, data));
    _instrument_state_handler->addInstrumentStateSet(data);

    return hasData(name);
}

int DataHandler::numData() const
{
    return _data_map.size();
}

bool DataHandler::hasData(const std::string& name) const
{
    return (_data_map.find(name) != _data_map.end());
}

void DataHandler::removeData(const std::string& name)
{
    if (auto it = _data_map.find(name); it != _data_map.end())
        _data_map.erase(it);
}

} // namespace nsx
