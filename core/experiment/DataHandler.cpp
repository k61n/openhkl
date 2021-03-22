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
#include "core/instrument/Diffractometer.h"
#include <iostream>

namespace nsx {

DataHandler::DataHandler(const std::string& name, const std::string& diffractometerName)
{
    _diffractometer = nullptr;
    if (!(diffractometerName == std::string("unknown_instrument")))
        _diffractometer.reset(Diffractometer::create(diffractometerName));
    _name = name;
}

Diffractometer* DataHandler::getDiffractometer()
{
    return _diffractometer.get();
}

void DataHandler::setDiffractometer(const std::string& diffractometerName)
{
    _diffractometer.reset(Diffractometer::create(diffractometerName));
}

DataHandler::DataHandler(const DataHandler& other)
{
    this->operator=(other);
}

DataHandler& DataHandler::operator=(const DataHandler& other)
{
    _data_map = other._data_map;
    _diffractometer.reset(other._diffractometer->clone());
    _name = other._name;
    return *this;
}

const DataMap* DataHandler::getDataMap() const
{
    return &_data_map;
}

const std::string DataHandler::getName() const
{
    return _name;
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
            "The data " + name + " could not be found in the experiment " + _name);
    }
    return it->second;
}

void DataHandler::addData(sptrDataSet data, std::string name)
{
    if (name == "")
        name = data->filename();

    // Add the data only if it does not exist in the current data map
    if (_data_map.find(name) != _data_map.end())
        return;

    const auto& metadata = data->reader()->metadata();

    const std::string diffName = metadata.key<std::string>("Instrument");

    if (!(diffName.compare(_diffractometer->name()) == 0)) {
        throw std::runtime_error("Mismatch between the diffractometer assigned to "
                                 "the experiment and the data");
    }
    const double wav = metadata.key<double>("wavelength");

    // ensure that there is at least one monochromator!
    if (_diffractometer->source().nMonochromators() == 0) {
        Monochromator mono("mono");
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
        Level::Info, "DataHandler::addData: adding DataSet ", name, ":", data->nFrames(), "frames");
    _data_map.insert(std::make_pair(name, data));
}

sptrDataSet DataHandler::dataShortName(std::string name) const
{
    for (DataMap::const_iterator it = _data_map.begin(); it != _data_map.end(); ++it) {
        if (it->second->name() == name)
            return it->second;
    }
    throw std::runtime_error("The data " + name + " could not be found in the experiment " + _name);
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
