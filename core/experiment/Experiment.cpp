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
    : _name(name), _data_map()
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

const Diffractometer* Experiment::diffractometer() const
{
    return _diffractometer.get();
}

Diffractometer* Experiment::diffractometer()
{
    return _diffractometer.get();
}

const std::map<std::string, sptrDataSet>& Experiment::getDataMap() const
{
    return _data_map;
}

DataList Experiment::getAllData()
{
    DataList numors;
    for (auto const& [key, val] : _data_map)
        numors.push_back(val);
    return numors;
}

void Experiment::setDiffractometer(const std::string& diffractometerName)
{
    _diffractometer.reset(Diffractometer::create(diffractometerName));
}

sptrDataSet Experiment::getData(const std::string& name)
{
    auto it = _data_map.find(name);
    if (it == _data_map.end())
        throw std::runtime_error(
            "The data " + name + " could not be found in the experiment " + _name);
    return it->second;
}

sptrDataSet Experiment::dataShortName(const std::string& name)
{
    std::map<std::string, sptrDataSet> temp;
    for (const auto& it : _data_map)
        temp.insert(std::make_pair(it.second->name(), it.second));

    const auto it = temp.find(name);
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
    if (_data_map.find(filename) != _data_map.end())
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

    if (_data_map.empty())
        mono.setWavelength(wav);
    else {
        if (std::abs(wav - mono.wavelength()) > 1e-5)
            throw std::runtime_error("trying to mix data with different wavelengths");
    }
    _data_map.insert(std::make_pair(filename, data));
}

void Experiment::addData(const std::string& name, sptrDataSet data)
{
    auto filename = data->filename();

    // Add the data only if it does not exist in the current data map
    if (_data_map.find(filename) != _data_map.end())
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

    if (_data_map.empty())
        mono.setWavelength(wav);
    else {
        if (std::abs(wav - mono.wavelength()) > 1e-5)
            throw std::runtime_error("trying to mix data with different wavelengths");
    }

    data->setName(name);
    _data_map.insert(std::make_pair(name, data));
}

bool Experiment::hasData(const std::string& name) const
{
    return (_data_map.find(name) != _data_map.end());
}

void Experiment::removeData(const std::string& name)
{
    auto it = _data_map.find(name);
    if (it != _data_map.end())
        _data_map.erase(it);
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
    if (!hasPeakCollection(name))
        return;

    auto peak_collection = _peak_collections.find(name);
    peak_collection->second.reset();
    _peak_collections.erase(peak_collection);
}

std::vector<std::string> Experiment::getCollectionNames() const
{
    std::vector<std::string> ret;
    for (const auto& it : _peak_collections)
        ret.push_back(it.second->name());
    return ret;
}

std::vector<std::string> Experiment::getFoundCollectionNames() const
{
    std::vector<std::string> ret;
    for (const auto& it : _peak_collections) {
        if (it.second->type() == listtype::FOUND)
            ret.push_back(it.second->name());
    }
    return ret;
}

std::vector<std::string> Experiment::getPredictedCollectionNames() const
{
    std::vector<std::string> ret;
    for (const auto& it : _peak_collections) {
        if (it.second->type() == listtype::PREDICTED)
            ret.push_back(it.second->name());
    }
    return ret;
}

void Experiment::acceptFilter(std::string name, PeakCollection* collection)
{
    auto ptr = std::make_unique<PeakCollection>(name, collection->type());
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
    auto ptr = std::make_unique<UnitCell>(*unit_cell);
    _unit_cells.insert({name, std::move(ptr)});
}

void Experiment::addUnitCell(
    const std::string& name, double a, double b, double c, double alpha, double beta, double gamma)
{
    auto cell = UnitCell(a, b, c, alpha * deg, beta * deg, gamma * deg);
    addUnitCell(name, &cell);
}

bool Experiment::hasUnitCell(const std::string& name) const
{
    auto unit_cell = _unit_cells.find(name);
    return (unit_cell != _unit_cells.end());
}

std::vector<std::string> Experiment::getUnitCellNames() const
{
    std::vector<std::string> ret;
    for (const auto& it : _unit_cells)
        ret.push_back(it.second->name());
    return ret;
}

const UnitCell* Experiment::getUnitCell(const std::string& name) const
{
    if (hasUnitCell(name))
        return _unit_cells.at(name).get();
    return nullptr;
}

UnitCell* Experiment::getUnitCell(const std::string& name)
{
    if (hasUnitCell(name))
        return _unit_cells.at(name).get();
    return nullptr;
}

void Experiment::removeUnitCell(const std::string& name)
{
    if (!hasUnitCell(name))
        return;
    auto unit_cell = _unit_cells.find(name);
    unit_cell->second.reset();
    _unit_cells.erase(unit_cell);
}

void Experiment::swapUnitCells(const std::string& old_cell_name, const std::string& new_cell_name)
{
    const UnitCell* old_cell = getUnitCell(old_cell_name);
    const UnitCell* new_cell = getUnitCell(new_cell_name);

    for (const auto& it : _peak_collections) {
        std::vector<Peak3D*> peaks = it.second.get()->getPeakList();
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
    for (const auto& it : _integrator_map) {
        if (it.first == name)
            return it.second.get();
    }
    return nullptr;
}

void Experiment::integratePeaks(const std::string& integrator_name, PeakCollection* peak_collection)
{
    IPeakIntegrator* integrator = getIntegrator(integrator_name);

    nsx::PeakFilter filter;
    filter.resetFiltering(peak_collection);
    filter.setDRange(std::array<double, 2UL>{integrator->dMin(), integrator->dMax()});
    filter.filterDRange(peak_collection);
    std::vector<Peak3D*> peaks = peak_collection->getFilteredPeakList();

    for (const auto& it : _data_map)
        integrator->integrate(peaks, peak_collection->shapeLibrary(), it.second);
}

void Experiment::integratePredictedPeaks(
    const std::string& integrator_name, PeakCollection* peak_collection,
    ShapeLibrary* shape_library, PredictionParameters& params)
{
    IPeakIntegrator* integrator = getIntegrator(integrator_name);
    integrator->setBkgBegin(params.bkg_begin);
    integrator->setBkgEnd(params.bkg_end);
    integrator->setDMin(params.detector_range_min);
    integrator->setDMax(params.detector_range_max);
    integrator->setRadius(params.neighbour_max_radius);
    integrator->setNFrames(params.frame_range_max);
    integrator->setFitCenter(params.set_fit_center);
    integrator->setFitCov(params.fit_covariance);
    nsx::PeakFilter filter;
    filter.resetFiltering(peak_collection);
    filter.setDRange(std::array<double, 2UL>{integrator->dMin(), integrator->dMax()});
    filter.filterDRange(peak_collection);
    std::vector<Peak3D*> peaks = peak_collection->getFilteredPeakList();

    for (const auto& it : _data_map)
        integrator->integrate(peaks, shape_library, it.second);
}

void Experiment::integrateFoundPeaks(const std::string& integrator_name)
{
    IPeakIntegrator* integrator = getIntegrator(integrator_name);

    for (sptrDataSet data : _peak_finder->currentData())
        integrator->integrate(_peak_finder->currentPeaks(), nullptr, data);
}

void Experiment::saveToFile(const std::string& path) const
{
    nsx::ExperimentExporter exporter;

    exporter.createFile(name(), _diffractometer->name(), path);

    std::map<std::string, DataSet*> data_sets;
    for (const auto& it : _data_map)
        data_sets.insert(std::make_pair(it.first, it.second.get()));
    exporter.writeData(data_sets);

    std::map<std::string, PeakCollection*> peak_collections;
    for (const auto& it : _peak_collections)
        peak_collections.insert(std::make_pair(it.first, it.second.get()));
    exporter.writePeaks(peak_collections);

    std::map<std::string, UnitCell*> unit_cells;
    for (const auto& it : _unit_cells)
        unit_cells.insert(std::make_pair(it.first, it.second.get()));
    exporter.writeUnitCells(unit_cells);

    exporter.finishWrite();
}

void Experiment::loadFromFile(const std::string& path)
{
    nsx::ExperimentImporter importer;

    importer.setFilePath(path, this);
    importer.loadData(this);
    importer.loadUnitCells(this);
    importer.loadPeaks(this);
}

void Experiment::setReferenceCell(
    double a, double b, double c, double alpha, double beta, double gamma)
{
    std::string name = "reference";
    auto reference_cell = UnitCell(a, b, c, alpha * deg, beta * deg, gamma * deg);
    addUnitCell(name, &reference_cell);
    _auto_indexer->setReferenceCell(getUnitCell(name));
}

bool Experiment::acceptUnitCell(PeakCollection* peaks, double length_tol, double angle_tol)
{
    std::string name = "accepted";
    bool accepted = false;
    if (_auto_indexer->hasSolution(length_tol, angle_tol)) {
        auto cell = *_auto_indexer->getAcceptedSolution();
        addUnitCell(name, &cell);
        acceptUnitCell(peaks);
        accepted = true;
    }
    return accepted;
}

std::vector<std::string> Experiment::getCompatibleSpaceGroups() const
{
    std::string cell_name = "accepted";
    const UnitCell* cell = getUnitCell(cell_name);
    return cell->compatibleSpaceGroups();
}

void Experiment::acceptUnitCell(PeakCollection* peaks)
{
    std::string name = "accepted";
    std::vector<Peak3D*> peak_list = peaks->getPeakList();
    for (auto peak : peak_list)
        peak->setUnitCell(getUnitCell(name));
}

void Experiment::buildShapeLibrary(PeakCollection* peaks, ShapeLibParameters params)
{
    std::vector<Peak3D*> peak_list = peaks->getPeakList();
    std::vector<Peak3D*> fit_peaks;

    for (nsx::Peak3D* peak : peak_list) {
        if (!peak->enabled())
            continue;
        const double d = 1.0 / peak->q().rowVector().norm();
        if (d > params.detector_range_max || d < params.detector_range_min)
            continue;

        const nsx::Intensity& intensity = peak->correctedIntensity();

        if (intensity.value() <= params.strength_min * intensity.sigma())
            continue;
        fit_peaks.push_back(peak);
    }
    if (fit_peaks.size() == 0)
        throw std::runtime_error("buildShapeLibrary: no fit peaks found");

    std::cout << peak_list.size() << " found peaks" << std::endl;
    std::cout << fit_peaks.size() << " fit peaks" << std::endl;

    nsx::AABB aabb;

    if (params.kabsch_coords) {
        const Eigen::Vector3d sigma(
            params.sigma_divergence, params.sigma_divergence, params.sigma_mosaicity);
        aabb.setLower(-params.peak_scale * sigma);
        aabb.setUpper(params.peak_scale * sigma);
    } else {
        const Eigen::Vector3d dx(params.nbins_x, params.nbins_y, params.nbins_z);
        aabb.setLower(-0.5 * dx);
        aabb.setUpper(0.5 * dx);
    }

    ShapeLibrary shape_library = ShapeLibrary(
        !params.kabsch_coords, params.peak_scale, params.background_range_min,
        params.background_range_max);

    nsx::ShapeIntegrator integrator(
        &shape_library, aabb, params.nbins_x, params.nbins_y, params.nbins_z);
    integrator.setPeakEnd(params.peak_scale);
    integrator.setBkgBegin(params.background_range_min);
    integrator.setBkgEnd(params.background_range_max);

    // TODO: (zamaan) change numors to a argument of buildShapeLibrary
    // Right now, there is no metadata for which DataSet was used to
    // Generate the peak collection
    for (auto const& [key, data] : _data_map)
        integrator.integrate(fit_peaks, &shape_library, data);

    shape_library = *integrator.library();
    peaks->setShapeLibrary(shape_library);
    // shape_library.updateFit(1000); // This does nothing!! - zamaan
}

void Experiment::predictPeaks(
    const std::string& name, PeakCollection* peaks, PredictionParameters params,
    PeakInterpolation interpol)
{
    const DataList numors = getAllData();
    std::vector<nsx::Peak3D*> predicted_peaks;
    const UnitCell* accepted_cell = getUnitCell("accepted");
    const ShapeLibrary* library = peaks->shapeLibrary();

    int current_numor = 0;
    for (const sptrDataSet& data : numors) {
        std::cout << "Predicting peaks for numor " << ++current_numor << " of " << numors.size()
                  << std::endl;

        const std::vector<nsx::Peak3D*> predicted = nsx::predictPeaks(
            library, data, accepted_cell, params.detector_range_min, params.detector_range_max,
            params.neighbour_max_radius, params.frame_range_max, params.min_n_neighbors, interpol);

        for (nsx::Peak3D* peak : predicted)
            predicted_peaks.push_back(peak);

        std::cout << "Completed  peak prediction. Added " << predicted_peaks.size() << " peaks";

        updatePeakCollection(name, listtype::PREDICTED, predicted_peaks);
        predicted_peaks.clear();
    }
}

void Experiment::computeQuality(
    double d_min, double d_max, int n_shells, PeakCollection* predicted, PeakCollection* found,
    bool friedel)
{
    ResolutionShell resolution_shell = nsx::ResolutionShell(d_min, d_max, n_shells);
    for (const auto& peak : found->getPeakList())
        resolution_shell.addPeak(peak);
    for (const auto& peak : predicted->getPeakList())
        resolution_shell.addPeak(peak);

    for (int i = n_shells - 1; i >= 0; --i) {
        double d_lower = resolution_shell.shell(i).dmin;
        double d_upper = resolution_shell.shell(i).dmax;

        nsx::MergedData merged_data_per_shell(_merged_peaks->spaceGroup(), friedel);

        for (const auto& peak : resolution_shell.shell(i).peaks)
            merged_data_per_shell.addPeak(peak);

        nsx::RFactor rf;
        rf.calculate(&merged_data_per_shell);
        nsx::CC cc;
        cc.calculate(&merged_data_per_shell);

        _data_resolution.push_back(
            {d_lower,
             d_upper,
             {rf.Rmerge(), rf.Rmeas(), rf.Rpim(), cc.CChalf()},
             {rf.expectedRmerge(), rf.expectedRmeas(), rf.expectedRpim(), cc.CCstar()}});
    }

    nsx::RFactor rf;
    rf.calculate(_merged_peaks.get());
    nsx::CC cc;
    cc.calculate(_merged_peaks.get());
    _data_quality_current = {rf.Rmerge(), rf.Rmeas(), rf.Rpim(), cc.CChalf()};
    _data_quality_expected = {rf.expectedRmerge(), rf.expectedRmeas(), rf.expectedRpim(),
                              cc.CCstar()};
}

const UnitCell* Experiment::getAcceptedCell() const
{
    return getUnitCell("accepted");
}

const UnitCell* Experiment::getReferenceCell() const
{
    return getUnitCell("reference");
}

void Experiment::refine(const PeakCollection* peaks, UnitCell* cell, DataSet* data, int n_batches)
{
    const unsigned int max_iter = 1000;
    const std::vector<Peak3D*> peak_list = peaks->getPeakList();
    InstrumentStateList& states = data->instrumentStates();
    Refiner refiner(states, cell, peak_list, n_batches);
    refiner.refine(max_iter);
}

void Experiment::checkPeakCollections()
{
    for (const auto& [name, collection] : _peak_collections)
        collection->checkCollection();
}

} // namespace nsx
