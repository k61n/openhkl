//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/instrument/Experiment.cpp
//! @brief     Defines ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include <iostream>
#include <stdexcept>
#include <utility>

#include "DataSet.h"
#include "Diffractometer.h"
#include "Experiment.h"
#include "IDataReader.h"
#include "MetaData.h"
#include "Monochromator.h"
#include "Source.h"

namespace nsx {

Experiment::Experiment(const std::string& name, const std::string& diffractometerName)
    : _name(name), _data()
{
    _diffractometer.reset(Diffractometer::create(diffractometerName));
}

Experiment::Experiment(const Experiment& other)
{
    _name = other._name;
    _data = other._data;
    _diffractometer.reset(other._diffractometer->clone());
}

Experiment& Experiment::operator=(const Experiment& other)
{
    if (this != &other) {
        _name = other._name;
        _data = other._data;
        _diffractometer.reset(other._diffractometer->clone());
    }
    return *this;
}

const Diffractometer* Experiment::diffractometer() const
{
    return _diffractometer.get();
}

Diffractometer* Experiment::diffractometer()
{
    return _diffractometer.get();
}

const std::map<std::string, sptrDataSet>& Experiment::data() const
{
    return _data;
}

sptrDataSet Experiment::data(std::string name)
{
    auto it = _data.find(name);
    if (it == _data.end()) {
        throw std::runtime_error(
            "The data " + name + " could not be found in the experiment " + _name);
    }
    return it->second;
}

const std::string& Experiment::name() const
{
    return _name;
}

void Experiment::setName(const std::string& name)
{
    _name = name;
}

void Experiment::addData(sptrDataSet data)
{
    auto filename = data->filename();

    // Add the data only if it does not exist in the current data map
    if (_data.find(filename) != _data.end()) {
        return;
    }

    const auto& metadata = data->reader()->metadata();

    std::string diffName = metadata.key<std::string>("Instrument");

    if (!(diffName.compare(_diffractometer->name()) == 0)) {
        throw std::runtime_error("Mismatch between the diffractometers assigned to "
                                 "the experiment and the data");
    }
    double wav = metadata.key<double>("wavelength");

    // ensure that there is at least one monochromator!
    if (_diffractometer->source().nMonochromators() == 0) {
        Monochromator mono("mono");
        _diffractometer->source().addMonochromator(mono);
    }

    auto& mono = _diffractometer->source().selectedMonochromator();

    if (_data.empty()) {
        mono.setWavelength(wav);
    } else {
        if (std::abs(wav - mono.wavelength()) > 1e-5)
            throw std::runtime_error("trying to mix data with different wavelengths");
    }
    _data.insert(std::make_pair(filename, data));
}

bool Experiment::hasData(const std::string& name) const
{
    auto it = _data.find(name);
    return (it != _data.end());
}

void Experiment::removeData(const std::string& name)
{
    auto it = _data.find(name);
    if (it != _data.end()) {
        _data.erase(it);
    }
}

} // end namespace nsx
