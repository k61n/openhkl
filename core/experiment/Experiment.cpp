//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/experiment/Experiment.cpp
//! @brief     Implements class Experiment
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

#include "core/experiment/DataSet.h"
#include "core/experiment/Experiment.h"
#include "core/instrument/Diffractometer.h"
#include "core/instrument/Monochromator.h"
#include "core/instrument/Source.h"
#include "core/raw/IDataReader.h"
#include "core/raw/MetaData.h"

#include "core/output/ExperimentExporter.h"

namespace nsx {

Experiment::Experiment(const std::string& name, const std::string& diffractometerName)
    : _name(name), _data()
{
    _diffractometer.reset(Diffractometer::create(diffractometerName));

    _peak_finder = new nsx::PeakFinder();
    _found_peak_integrator = new nsx::PixelSumIntegrator(true, true);
}

Experiment::Experiment(const Experiment& other)
{
    _name = other._name;
    _data = other._data;
    _diffractometer.reset(other._diffractometer->clone());

    _peak_finder = new nsx::PeakFinder();
    _found_peak_integrator = new nsx::PixelSumIntegrator(true, true);
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
    if (_data.find(filename) != _data.end())
        return;

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

    if (_data.empty())
        mono.setWavelength(wav);
    else {
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
    if (it != _data.end())
        _data.erase(it);
}

void Experiment::addPeakCollection(
    const std::string name, 
    const std::vector<std::shared_ptr<nsx::Peak3D>>* peaks) {
    nsx::listtype type{listtype::FOUND};
    std::unique_ptr<PeakCollection> ptr(new PeakCollection(name, type));
    ptr->populate(peaks);
    
    _peakCollections.insert(std::make_pair(name, std::move(ptr)));
}

bool Experiment::hasPeakCollection(const std::string& name) const {
    auto peaks = _peakCollections.find(name);
    return (peaks != _peakCollections.end());
}

PeakCollection* Experiment::getPeakCollection(const std::string name){
    return _peakCollections[name].get();
}

void Experiment::removePeakCollection(const std::string& name) {
    auto peaks = _peakCollections.find(name);
    if (peaks != _peakCollections.end())
        _peakCollections.erase(peaks);
}

std::vector<std::string*> Experiment::getCollectionNames() const {
    
    std::vector<std::string*> names;
	for (
        std::map<std::string,std::unique_ptr<PeakCollection>>::const_iterator it = _peakCollections.begin(); 
        it != _peakCollections.end(); ++it) {
		names.push_back(it->second->name());
	}
    return names;
}

void Experiment::addUnitCell(const std::string& name, sptrUnitCell unit_cell) {
    _unit_cells.insert(std::make_pair(name, unit_cell));
}

bool Experiment::hasUnitCell(const std::string& name) const {
    auto unit_cell = _unit_cells.find(name);
    return (unit_cell != _unit_cells.end());
}

sptrUnitCell Experiment::getUnitCell(const std::string& name) {
    return _unit_cells[name];
}

void Experiment::removeUnitCell(const std::string& name) {
    auto unit_cell = _unit_cells.find(name);
    if (unit_cell != _unit_cells.end())
        _unit_cells.erase(unit_cell);
}

void Experiment::acceptFoundPeaks(const std::string& name) {
    addPeakCollection(name, _peak_finder->currentPeaks());
}

void Experiment::integrateFoundPeaks(
    double peak_end, double bkg_begin, double bkg_end)
{   
    for (nsx::sptrDataSet data : _peak_finder->currentData())
    {
        _found_peak_integrator->integrate(
            *_peak_finder->currentPeaks(), data, 
            peak_end, bkg_begin,  bkg_end);
    }
}

bool Experiment::saveToFile(std::string path) const
{
    nsx::ExperimentExporter exporter;

    bool success = exporter.createFile(path);

    if (success){
        std::map<std::string,DataSet*> data_sets;
        for (
            std::map<std::string,sptrDataSet>::const_iterator it = _data.begin(); 
            it != _data.end(); ++it) {
            data_sets.insert(std::make_pair(it->first,it->second.get()));
        }
        success = exporter.writeData(data_sets);
    }

    if (success){
        std::map<std::string,PeakCollection*> peak_collections;
        for (
            std::map<std::string,std::unique_ptr<PeakCollection>>::const_iterator it = _peakCollections.begin(); 
            it != _peakCollections.end(); ++it) {
            peak_collections.insert(std::make_pair(it->first,it->second.get()));
        }
        success = exporter.writePeaks(peak_collections);
    }

    if (success)
        success = exporter.finishWrite();

    return success;
}


} // namespace nsx
