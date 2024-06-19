//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/experiment/DataHandler.cpp
//! @brief     Handles DataSet manipulations for Experiment object
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "core/experiment/DataHandler.h"

#include "base/geometry/AABB.h"
#include "base/utils/Logger.h"
#include "core/data/DataSet.h"
#include "core/experiment/InstrumentStateHandler.h"
#include "core/instrument/Diffractometer.h"
#include "core/raw/DataKeys.h"

#include <stdexcept>

namespace ohkl {

DataHandler::DataHandler(
    const std::string& experiment_name, InstrumentStateHandler* instrument_state_handler)
    : _experiment_name{experiment_name}, _instrument_state_handler(instrument_state_handler)
{
}

const DataMap* DataHandler::getDataMap() const
{
    return &_data_map;
}

DataList DataHandler::getAllData() const
{
    DataList datasets;
    for (auto const& [key, val] : _data_map)
        datasets.push_back(val);
    return datasets;
}

sptrDataSet DataHandler::getData(std::string name) const
{
    auto it = _data_map.find(name);
    if (it == _data_map.end()) {
        throw std::runtime_error(
            "The DataSet name " + name + " could not be found in the experiment "
            + _experiment_name);
    }
    return it->second;
}

bool DataHandler::addData(
    sptrDataSet data, Diffractometer* diffractometer, std::string name, bool default_states)
{
    if (name.empty())
        name = data->name();

    if (name.empty())
        throw std::invalid_argument("DataHandler::addData: Data name cannot be empty");

    if (hasData(name))
        throw std::invalid_argument("DataHandler::addData: Data name must be unique");

    // Add the data only if it does not exist in the current data map
    if (_data_map.find(name) != _data_map.end())
        throw std::invalid_argument(
            "DataHandler::addData: Data name '" + name + "' already exists.");

    const auto& metadata = data->metadata();

    const std::string diffName = metadata.key<std::string>(ohkl::at_diffractometer);

    if (!(diffName.compare(diffractometer->name()) == 0)) {
        throw std::runtime_error(
            "Mismatch between the diffractometer assigned to "
            "the experiment, '"
            + diffractometer->name()
            + "', "
              "and the data, '"
            + diffName + "'");
    }
    const double wav = metadata.key<double>(ohkl::at_wavelength);

    auto& mono = diffractometer->source().selectedMonochromator();

    if (_data_map.empty())
        mono.setWavelength(wav);
    else {
        if (std::abs(wav - mono.wavelength()) > 1e-5)
            throw std::runtime_error("trying to mix data with different wavelengths");
    }

    ohklLog(
        Level::Info, "DataHandler::addData: adding DataSet '", name, "': ", data->nFrames(),
        " frames");
    _data_map.insert({name, data});
    if (default_states)
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

} // namespace ohkl
