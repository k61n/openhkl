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
#include <memory>
#include <stdexcept>
#include <utility>

#include "base/utils/Units.h"

#include "core/data/DataSet.h"
#include "core/experiment/Experiment.h"
#include "core/instrument/Diffractometer.h"
#include "core/instrument/Monochromator.h"
#include "core/instrument/Source.h"
#include "core/raw/IDataReader.h"
#include "core/raw/MetaData.h"

#include "core/experiment/ExperimentExporter.h"
#include "core/experiment/ExperimentImporter.h"

#include "core/integration/GaussianIntegrator.h"
#include "core/integration/ISigmaIntegrator.h"
#include "core/integration/PixelSumIntegrator.h"
#include "core/integration/Profile1DIntegrator.h"
#include "core/integration/Profile3DIntegrator.h"
#include "core/integration/ShapeIntegrator.h"

namespace nsx {

Experiment::Experiment(const std::string& name, const std::string& diffractometerName)
    : _name(name), _data()
{
    _diffractometer.reset(Diffractometer::create(diffractometerName));

    _peak_finder = std::make_unique<PeakFinder>();
    _peak_filter = std::make_unique<PeakFilter>();
    _auto_indexer = std::make_unique<AutoIndexer>();

    _integrator_map.insert(std::make_pair(
        std::string("Pixel sum integrator"), std::make_unique<PixelSumIntegrator>(true, true)));

    _integrator_map.insert(std::make_pair(
        std::string("Gaussian integrator"), std::make_unique<GaussianIntegrator>(true, true)));

    _integrator_map.insert(
        std::make_pair(std::string("I/Sigma integrator"), std::make_unique<ISigmaIntegrator>()));

    _integrator_map.insert(std::make_pair(
        std::string("1d profile integrator"), std::make_unique<Profile1DIntegrator>()));

    _integrator_map.insert(std::make_pair(
        std::string("3d profile integrator"), std::make_unique<Profile3DIntegrator>()));
}

Experiment::Experiment(const Experiment& other)
{
    _name = other._name;
    _data = other._data;
    _diffractometer.reset(other._diffractometer->clone());

    _peak_finder = std::make_unique<PeakFinder>();
    _peak_filter = std::make_unique<PeakFilter>();
    _auto_indexer = std::make_unique<AutoIndexer>();

    _integrator_map.insert(std::make_pair(
        std::string("Pixel sum integrator"), std::make_unique<PixelSumIntegrator>(true, true)));

    _integrator_map.insert(std::make_pair(
        std::string("Gaussian integrator"), std::make_unique<GaussianIntegrator>(true, true)));

    _integrator_map.insert(
        std::make_pair(std::string("I/Sigma integrator"), std::make_unique<ISigmaIntegrator>()));

    _integrator_map.insert(std::make_pair(
        std::string("1d profile integrator"), std::make_unique<Profile1DIntegrator>()));

    _integrator_map.insert(std::make_pair(
        std::string("3d profile integrator"), std::make_unique<Profile3DIntegrator>()));
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

const std::map<std::string, sptrDataSet>& Experiment::getData() const
{
    return _data;
}

void Experiment::setDiffractometer(const std::string& diffractometerName)
{
    _diffractometer.reset(Diffractometer::create(diffractometerName));
}

sptrDataSet Experiment::getData(std::string name)
{
    auto it = _data.find(name);
    if (it == _data.end()) {
        throw std::runtime_error(
            "The data " + name + " could not be found in the experiment " + _name);
    }
    return it->second;
}

sptrDataSet Experiment::dataShortName(std::string name)
{
    std::map<std::string, sptrDataSet> temp;
    for (std::map<std::string, sptrDataSet>::iterator it = _data.begin(); it != _data.end(); ++it) {
        temp.insert(std::make_pair(it->second->name(), it->second));
    }

    auto it = temp.find(name);
    if (it == temp.end()) {
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
        throw std::runtime_error("Mismatch between the diffractometer assigned to "
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

void Experiment::addData(std::string name, sptrDataSet data)
{
    auto filename = data->filename();

    // Add the data only if it does not exist in the current data map
    if (_data.find(filename) != _data.end())
        return;

    const auto& metadata = data->reader()->metadata();

    std::string diffName = metadata.key<std::string>("Instrument");

    if (!(diffName.compare(_diffractometer->name()) == 0)) {
        throw std::runtime_error("Mismatch between the diffractometer assigned to "
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

    data->setName(name);
    _data.insert(std::make_pair(name, data));
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

void Experiment::updatePeakCollection(
    const std::string& name, const listtype type, const std::vector<nsx::Peak3D*> peaks)
{
    std::unique_ptr<PeakCollection> ptr(new PeakCollection(name, type));
    ptr->populate(peaks);
    _peak_collections.insert_or_assign(name, std::move(ptr));
}

bool Experiment::hasPeakCollection(const std::string& name) const
{
    auto peaks = _peak_collections.find(name);
    return (peaks != _peak_collections.end());
}

PeakCollection* Experiment::getPeakCollection(const std::string name)
{
    std::cout << hasPeakCollection(name) << " " << name << std::endl;
    if (hasPeakCollection(name))
        return _peak_collections[name].get();
    return nullptr;
}

void Experiment::removePeakCollection(const std::string& name)
{
    if (hasPeakCollection(name)) {
        auto peak_collection = _peak_collections.find(name);
        peak_collection->second.reset();
        _peak_collections.erase(peak_collection);
    }
}

std::vector<std::string> Experiment::getCollectionNames() const
{

    std::vector<std::string> names;
    for (std::map<std::string, std::unique_ptr<PeakCollection>>::const_iterator it =
             _peak_collections.begin();
         it != _peak_collections.end(); ++it) {
        names.push_back(it->second->name());
    }
    return names;
}

std::vector<std::string> Experiment::getFoundCollectionNames() const
{

    std::vector<std::string> names;
    for (std::map<std::string, std::unique_ptr<PeakCollection>>::const_iterator it =
             _peak_collections.begin();
         it != _peak_collections.end(); ++it) {
        if (it->second->type() == listtype::FOUND)
            names.push_back(it->second->name());
    }
    return names;
}

std::vector<std::string> Experiment::getPredictedCollectionNames() const
{

    std::vector<std::string> names;
    for (std::map<std::string, std::unique_ptr<PeakCollection>>::const_iterator it =
             _peak_collections.begin();
         it != _peak_collections.end(); ++it) {
        if (it->second->type() == listtype::PREDICTED)
            names.push_back(it->second->name());
    }
    return names;
}

void Experiment::acceptFilter(std::string name, PeakCollection* collection)
{
    std::unique_ptr<PeakCollection> ptr(new PeakCollection(name, collection->type()));
    ptr->populateFromFiltered(collection);
    _peak_collections.insert_or_assign(name, std::move(ptr));
}

void Experiment::setMergedPeaks(std::vector<PeakCollection*> peak_collections, bool friedel)
{
    _merged_peaks = std::make_unique<MergedData>(peak_collections, friedel);
}

void Experiment::setMergedPeaks(PeakCollection* found, PeakCollection* predicted, bool friedel)
{
    std::vector<PeakCollection*> collections;
    collections.push_back(found);
    collections.push_back(predicted);
    _merged_peaks = std::make_unique<MergedData>(collections, friedel);
}

void Experiment::resetMergedPeaks()
{
    _merged_peaks.reset();
}

void Experiment::addUnitCell(const std::string& name, UnitCell* unit_cell)
{
    std::unique_ptr<UnitCell> ptr(new UnitCell(*unit_cell));
    _unit_cells.insert(std::make_pair(name, std::move(ptr)));
}

bool Experiment::hasUnitCell(const std::string& name) const
{
    auto unit_cell = _unit_cells.find(name);
    return (unit_cell != _unit_cells.end());
}

std::vector<std::string> Experiment::getUnitCellNames() const
{
    std::vector<std::string> names;
    for (std::map<std::string, std::unique_ptr<nsx::UnitCell>>::const_iterator it =
             _unit_cells.begin();
         it != _unit_cells.end(); ++it) {
        names.push_back(it->second->name());
    }
    return names;
}

UnitCell* Experiment::getUnitCell(const std::string& name)
{
    if (hasUnitCell(name))
        return _unit_cells[name].get();
    return nullptr;
}

void Experiment::removeUnitCell(const std::string& name)
{
    if (hasUnitCell(name)) {
        auto unit_cell = _unit_cells.find(name);
        unit_cell->second.reset();
        _unit_cells.erase(unit_cell);
    }
}

void Experiment::swapUnitCells(const std::string& old_cell_name, const std::string& new_cell_name)
{
    UnitCell* old_cell = getUnitCell(old_cell_name);
    UnitCell* new_cell = getUnitCell(new_cell_name);

    std::map<std::string, std::unique_ptr<PeakCollection>>::const_iterator it;
    for (it = _peak_collections.begin(); it != _peak_collections.end(); ++it) {
        std::vector<Peak3D*> peaks = it->second.get()->getPeakList();
        for (Peak3D* peak : peaks) {
            if (peak->unitCell() == old_cell)
                peak->setUnitCell(new_cell);
        }
    }
}

void Experiment::acceptFoundPeaks(const std::string& name)
{
    std::vector<Peak3D*> peaks = _peak_finder->currentPeaks();
    updatePeakCollection(name, listtype::FOUND, peaks);
}

IPeakIntegrator* Experiment::getIntegrator(const std::string& name) const
{
    std::map<std::string, std::unique_ptr<IPeakIntegrator>>::const_iterator it;
    for (it = _integrator_map.begin(); it != _integrator_map.end(); ++it) {
        if (it->first == name)
            return it->second.get();
    }
    return nullptr;
}

void Experiment::integratePeaks(std::string integrator_name, PeakCollection* peak_collection)
{

    IPeakIntegrator* integrator = getIntegrator(integrator_name);

    nsx::PeakFilter filter;
    filter.resetFiltering(peak_collection);
    filter.setDRange(std::array<double, 2UL> {integrator->dMin(), integrator->dMax()});
    filter.filterDRange(peak_collection);
    std::vector<Peak3D*> peaks = peak_collection->getFilteredPeakList();

    std::map<std::string, sptrDataSet>::iterator it;
    for (it = _data.begin(); it != _data.end(); ++it) {
        integrator->integrate(peaks, peak_collection->shapeLibrary(), it->second);
    }
}

void Experiment::integratePredictedPeaks(
    std::string integrator_name, PeakCollection* peak_collection, ShapeLibrary* shape_library)
{
    IPeakIntegrator* integrator = getIntegrator(integrator_name);
    nsx::PeakFilter filter;
    filter.resetFiltering(peak_collection);
    filter.setDRange(std::array<double, 2UL> {integrator->dMin(), integrator->dMax()});
    filter.filterDRange(peak_collection);
    std::vector<Peak3D*> peaks = peak_collection->getFilteredPeakList();

    std::map<std::string, sptrDataSet>::iterator it;
    for (it = _data.begin(); it != _data.end(); ++it) {
        integrator->integrate(peaks, shape_library, it->second);
    }
}

void Experiment::integrateFoundPeaks(std::string integrator_name)
{
    IPeakIntegrator* integrator = getIntegrator(integrator_name);

    for (sptrDataSet data : _peak_finder->currentData()) {
        integrator->integrate(_peak_finder->currentPeaks(), nullptr, data);
    }
}

bool Experiment::saveToFile(std::string path) const
{
    nsx::ExperimentExporter exporter;

    bool success = exporter.createFile(name(), _diffractometer->name(), path);

    if (success) {
        std::map<std::string, DataSet*> data_sets;
        for (std::map<std::string, sptrDataSet>::const_iterator it = _data.begin();
             it != _data.end(); ++it) {
            data_sets.insert(std::make_pair(it->first, it->second.get()));
        }
        success = exporter.writeData(data_sets);
    }

    if (success) {
        std::map<std::string, PeakCollection*> peak_collections;
        for (std::map<std::string, std::unique_ptr<PeakCollection>>::const_iterator it =
                 _peak_collections.begin();
             it != _peak_collections.end(); ++it) {
            peak_collections.insert(std::make_pair(it->first, it->second.get()));
        }
        success = exporter.writePeaks(peak_collections);
    }

    if (success) {
        std::map<std::string, UnitCell*> unit_cells;
        for (std::map<std::string, std::unique_ptr<UnitCell>>::const_iterator it =
                 _unit_cells.begin();
             it != _unit_cells.end(); ++it) {
            unit_cells.insert(std::make_pair(it->first, it->second.get()));
        }
        success = exporter.writeUnitCells(unit_cells);
    }

    if (success)
        success = exporter.finishWrite();

    return success;
}

bool Experiment::loadFromFile(std::string path)
{
    nsx::ExperimentImporter importer;

    bool success = importer.setFilePath(path, this);

    if (success) {
        success = importer.loadData(this);
    }

    if (success) {
        success = importer.loadUnitCells(this);
    }

    if (success) {
        success = importer.loadPeaks(this);
    }
    return success;
}

void Experiment::setReferenceCell(
    double a, double b, double c, double alpha, double beta, double gamma)
{
    _reference_cell = UnitCell(a, b, c, alpha * deg, beta * deg, gamma * deg);
    _auto_indexer->setReferenceCell(&_reference_cell);
}

bool Experiment::acceptUnitCell(PeakCollection* peaks, double length_tol, double angle_tol)
{
    bool accepted = false;
    if (_auto_indexer->hasSolution(length_tol, angle_tol)) {
        _accepted_unit_cell = *_auto_indexer->getAcceptedSolution();
        std::vector<Peak3D*> peak_list = peaks->getPeakList();
        for (auto peak : peak_list)
            peak->setUnitCell(&_accepted_unit_cell);
        accepted = true;
    }
    return accepted;
}

UnitCell* Experiment::getAcceptedCell()
{
    return &_accepted_unit_cell;
}

void Experiment::buildShapeLibrary(
    PeakCollection* peaks, DataList numors, ShapeLibParameters params)
{
    std::vector<Peak3D*> peak_list = peaks->getPeakList();
    std::vector<Peak3D*> fit_peaks;

    for (nsx::Peak3D* peak : peak_list) {
        if (!peak->enabled())
            continue;
        double d = 1.0 / peak->q().rowVector().norm();

        if (d > params.detector_range_max || d < params.detector_range_min)
            continue;

        nsx::Intensity intensity = peak->correctedIntensity();

        if (intensity.value() <= params.strength_min * intensity.sigma())
            continue;
        fit_peaks.push_back(peak);
    }

    nsx::AABB aabb;

    if (params.kabsch_coords) {
        Eigen::Vector3d sigma(
            params.sigma_divergence, params.sigma_divergence, params.sigma_mosaicity);
        aabb.setLower(-params.peak_scale * sigma);
        aabb.setUpper(params.peak_scale * sigma);
    } else {
        Eigen::Vector3d dx(params.nbins_x, params.nbins_y, params.nbins_z);
        aabb.setLower(-0.5 * dx);
        aabb.setUpper(0.5 * dx);
    }

    _shape_library = nsx::ShapeLibrary(
        !params.kabsch_coords, params.peak_scale, params.background_range_min,
        params.background_range_max);

    nsx::ShapeIntegrator integrator(
        &_shape_library, aabb, params.nbins_x, params.nbins_y, params.nbins_z);
    integrator.setPeakEnd(params.peak_scale);
    integrator.setBkgBegin(params.background_range_min);
    integrator.setBkgEnd(params.background_range_max);

    for (auto data : numors)
        integrator.integrate(fit_peaks, &_shape_library, data);

    _shape_library = *integrator.library(); // why do this? - zamaan
    // _shape_library.updateFit(1000); // This does nothing!! - zamaan
}

void Experiment::predictPeaks(
    std::string name, DataList numors, PredictionParameters params, PeakInterpolation interpol)
{
    int current_numor = 0;
    std::vector<nsx::Peak3D*> predicted_peaks;

    for (auto data : numors) {
        std::cout << "Predicting peaks for numor " << ++current_numor << " of " << numors.size()
                  << std::endl;

        std::vector<nsx::Peak3D*> predicted = nsx::predictPeaks(
            &_shape_library, data, &_accepted_unit_cell, params.detector_range_min,
            params.detector_range_max, params.neighbour_max_radius, params.frame_range_max,
            params.min_n_neighbors, interpol);

        for (nsx::Peak3D* peak : predicted)
            predicted_peaks.push_back(peak);

        std::cout << "Added " << predicted.size() << " predicted peaks.";
    }
    std::cout << "Completed  peak prediction. Added " << predicted_peaks.size() << " peaks";

    updatePeakCollection(name, listtype::PREDICTED, predicted_peaks);

    for (nsx::Peak3D* peak : predicted_peaks) // is this necessary? - zamaan
        delete peak;
    predicted_peaks.clear();
}

void Experiment::computeQuality(
    double d_min, double d_max, int n_shells, PeakCollection* predicted, PeakCollection* found,
    bool friedel)
{
    ResolutionShell resolution_shell = nsx::ResolutionShell(d_min, d_max, n_shells);
    for (auto peak : found->getPeakList())
        resolution_shell.addPeak(peak);
    for (auto peak : predicted->getPeakList())
        resolution_shell.addPeak(peak);

    for (int i = n_shells - 1; i >= 0; --i) {
        double d_lower = resolution_shell.shell(i).dmin;
        double d_upper = resolution_shell.shell(i).dmax;

        nsx::MergedData merged_data_per_shell(_merged_peaks->spaceGroup(), friedel);

        for (auto peak : resolution_shell.shell(i).peaks)
            merged_data_per_shell.addPeak(peak);

        nsx::RFactor rf;
        rf.calculate(&merged_data_per_shell);
        nsx::CC cc;
        cc.calculate(&merged_data_per_shell);

        _data_resolution.push_back(
            {{rf.Rmerge(), rf.expectedRmerge(), rf.Rmeas(), rf.expectedRmeas(), rf.Rpim(),
              rf.expectedRpim(), cc.CChalf(), cc.CCstar()},
             d_lower,
             d_upper});
    }

    nsx::RFactor rfactor;
    rfactor.calculate(_merged_peaks.get());
    nsx::CC cc;
    cc.calculate(_merged_peaks.get());
    _data_quality = {
        rfactor.Rmerge(), rfactor.expectedRmerge(), rfactor.Rmeas(), rfactor.expectedRmeas(),
        rfactor.Rpim(),   rfactor.expectedRpim(),   cc.CChalf(),     cc.CCstar()};
}

} // namespace nsx
