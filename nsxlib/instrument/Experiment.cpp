#include <iostream>
#include <stdexcept>
#include <utility>

#include "DataSet.h"
#include "Diffractometer.h"
#include "Experiment.h"
#include "MetaData.h"
#include "Monochromator.h"
#include "Source.h"

namespace nsx {

Experiment::Experiment(const Experiment& other)
: _name(other._name),
  _diffractometerName(other._diffractometerName),
  _diffractometer(other._diffractometer),
  _data(other._data)
{
}

Experiment::Experiment(const std::string& name, const std::string& diffractometerName)
: _name(name),
  _diffractometerName(diffractometerName),
  _data()
{
    _diffractometer = Diffractometer::build(diffractometerName);
}

Experiment::Experiment(const std::string& diffractometerName)
: _name("experiment"),
  _diffractometerName(diffractometerName),
  _data()
{
    _diffractometer = Diffractometer::build(diffractometerName);
}

Experiment::~Experiment()
{
}

Experiment& Experiment::operator=(const Experiment& other)
{
    if (this != &other)
    {
        _name = other._name;
        _diffractometerName = other._diffractometerName;
        _diffractometer = other._diffractometer;
        _data = other._data;
    }
    return *this;
}

sptrDiffractometer Experiment::diffractometer() const
{
    return _diffractometer;
}

const std::string& Experiment::getDiffractometerType() const
{
    return _diffractometerName;
}

std::vector<std::string> Experiment::getDataNames() const
{
    std::vector<std::string> v;
    v.reserve(_data.size());
    std::for_each(_data.begin(),_data.end(),[&v]
                                             (const std::map<std::string,sptrDataSet>::value_type& p)
                                             {v.push_back(p.first);});

    return v;
}

const std::map<std::string,sptrDataSet>& Experiment::getData() const
{
    return _data;
}


sptrDataSet Experiment::getData(std::string name)
{
    auto it = _data.find(name);
    if (it == _data.end()) {
        throw std::runtime_error("The data "+name+" could not be found in the experiment "+_name);
    }
    return it->second;
}

const std::string& Experiment::getName() const
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
    std::string diffName = data->metadata()->getKey<std::string>("Instrument");

    if (!(diffName.compare(_diffractometer->name())==0)) {
        throw std::runtime_error("Mismatch between the diffractometers assigned to the experiment and the data");
    }
    double wav=data->metadata()->getKey<double>("wavelength");

    // ensure that there is at least one monochromator!
    if ( _diffractometer->source()->getNMonochromators() == 0 ) {
        Monochromator mono("mono");
        _diffractometer->source()->addMonochromator(mono);
    }

    auto& mono = _diffractometer->source()->getSelectedMonochromator();

    if (_data.empty()) {
        mono.setWavelength(wav);
    } else {
        if (std::abs(wav-mono.getWavelength())>1e-5)
            throw std::runtime_error("trying to mix data with different wavelengths");
    }
    _data.insert(std::make_pair(filename,data));
}

bool Experiment::hasData(const std::string& name) const
{
    auto it=_data.find(name);
    return (it != _data.end());
}

void Experiment::removeData(const std::string& name)
{
    auto it=_data.find(name);
    if (it != _data.end()) {
        _data.erase(it);
    }
}

} // end namespace nsx
