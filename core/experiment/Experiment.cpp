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

#include "base/utils/Logger.h"
#include "base/utils/Units.h"
#include "core/data/DataSet.h"
#include "core/experiment/DataHandler.h"
#include "core/experiment/Experiment.h"
#include "core/experiment/ExperimentExporter.h"
#include "core/experiment/ExperimentImporter.h"
#include "core/experiment/IntegrationHandler.h"
#include "core/experiment/PeakHandler.h"
#include "core/experiment/UnitCellHandler.h"
#include "core/instrument/Diffractometer.h"
#include "core/instrument/Monochromator.h"
#include "core/instrument/Source.h"
#include "core/integration/GaussianIntegrator.h"
#include "core/integration/ISigmaIntegrator.h"
#include "core/integration/PixelSumIntegrator.h"
#include "core/integration/Profile1DIntegrator.h"
#include "core/integration/Profile3DIntegrator.h"
#include "core/integration/ShapeIntegrator.h"
#include "core/peak/PeakCoordinateSystem.h"
#include "core/raw/IDataReader.h"
#include "core/raw/MetaData.h"
#include "core/statistics/CC.h"
#include "core/statistics/RFactor.h"
#include "core/raw/DataKeys.h"

namespace nsx {

Experiment::~Experiment() = default;

Experiment::Experiment(const std::string& name, const std::string& diffractometerName) : _name(name)
{
    // start logging
    Logger::instance().start( nsx::kw_logFilename, Level::Debug);
    _peak_finder = std::make_unique<PeakFinder>();
    _peak_filter = std::make_unique<PeakFilter>();
    _auto_indexer = std::make_unique<AutoIndexer>();
    _peak_merger = std::make_unique<PeakMerger>();

    _data_handler = std::make_shared<DataHandler>(_name, diffractometerName);
    _peak_handler = std::make_unique<PeakHandler>();
    _cell_handler = std::make_unique<UnitCellHandler>();
    _integration_handler = std::make_unique<IntegrationHandler>(_data_handler);

    _finder_params = std::make_shared<PeakFinderParameters>();
    _filter_params = std::make_shared<PeakFilterParameters>();
    _indexer_params = std::make_shared<IndexerParameters>();
    _predict_params = std::make_unique<PredictionParameters>();
    _shape_params= std::make_unique<ShapeCollectionParameters>();
    _refiner_params = std::make_shared<RefinerParameters>();
    _int_params = std::make_unique<IntegrationParameters>();
    _merge_params = std::make_shared<MergeParameters>();

    _peak_finder->setParameters(_finder_params);
    _peak_filter->setParameters(_filter_params);
    _auto_indexer->setParameters(_indexer_params);
    _peak_merger->setParameters(_merge_params);
}

const std::string& Experiment::name() const
{
    return _name;
}

void Experiment::setName(const std::string& name)
{
    _name = name;
}

void Experiment::setDefaultDMin()
{
    double lambda = getDiffractometer()->source().selectedMonochromator().wavelength();
    double d_min = lambda / 2.0;
    _shape_params->d_min = d_min;
    _predict_params->d_min = d_min;
    _indexer_params->d_min = d_min;
    _filter_params->d_min = d_min;
    _merge_params->d_min = d_min;
}

void Experiment::acceptFoundPeaks(const std::string& name)
{
    std::vector<Peak3D*> peaks = _peak_finder->currentPeaks();
    addPeakCollection(name, listtype::FOUND, peaks);
}

void Experiment::saveToFile(const std::string& path) const
{
    nsx::ExperimentExporter exporter;
    nsxlog(Level::Info, "Saving experiment to file: ", path);

    // AN>>TODO: if no extension given, use .nsx
    exporter.createFile(name(), getDiffractometer()->name(), path);

    std::map<std::string, DataSet*> data_sets;
    for (const auto& it : *_data_handler->getDataMap())
        data_sets.insert(std::make_pair(it.first, it.second.get()));
    exporter.writeData(data_sets);

    std::map<std::string, PeakCollection*> peak_collections;
    for (const auto& it : *_peak_handler->getPeakCollectionMap())
        peak_collections.insert(std::make_pair(it.first, it.second.get()));
    exporter.writePeaks(peak_collections);

    std::map<std::string, UnitCell*> unit_cells;
    for (const auto& it : *_cell_handler->getCellMap())
        unit_cells.insert(std::make_pair(it.first, it.second.get()));
    exporter.writeUnitCells(unit_cells);

    exporter.finishWrite();
}

void Experiment::loadFromFile(const std::string& path)
{
    nsx::ExperimentImporter importer;
    nsxlog(Level::Info, "Loading experiment from file: ", path);

    importer.setFilePath(path, this);
    importer.loadData(this);
    importer.loadUnitCells(this);
    importer.loadPeaks(this);
    setDefaultDMin();
}

void Experiment::autoIndex(PeakCollection* peaks)
{

    auto params = _auto_indexer->parameters();

    std::string collection_name = nsx::kw_autoindexingCollection;

    _peak_filter->resetFiltering(peaks);
    _peak_filter->resetFilterFlags();
    _peak_filter->flags()->strength = true;
    _peak_filter->flags()->d_range = true;
    _peak_filter->flags()->frames = true;
    _peak_filter->parameters()->d_min = params->d_min;
    _peak_filter->parameters()->d_max = params->d_max;
    _peak_filter->parameters()->strength_min = params->strength_min;
    _peak_filter->parameters()->strength_max = params->strength_max;
    _peak_filter->parameters()->frame_min = params->first_frame;
    _peak_filter->parameters()->frame_max = params->last_frame;

    nsxlog(
        Level::Info, "Experiment::autoIndex: attempting with frames ", params->first_frame, " - ",
        params->last_frame);
    _peak_filter->filter(peaks);
    double npeaks = peaks->numberOfPeaks();
    double ncaught = peaks->numberCaughtByFilter();
    nsxlog(Level::Info, "Indexing using ", ncaught, " / ", npeaks, " peaks");
    _peak_handler->acceptFilter(collection_name, peaks, listtype::INDEXING);
    PeakCollection* indexing_collection = getPeakCollection(collection_name);
    _auto_indexer->autoIndex(indexing_collection);
}

void Experiment::buildShapeCollection(
    PeakCollection* peaks, sptrDataSet data, const ShapeCollectionParameters& params)
{
    params.log(Level::Info);
    peaks->computeSigmas();

    _peak_filter->resetFiltering(peaks);
    _peak_filter->resetFilterFlags();
    _peak_filter->flags()->strength = true;
    _peak_filter->flags()->d_range = true;
    _peak_filter->parameters()->d_min = params.d_min;
    _peak_filter->parameters()->d_max = params.d_max;
    _peak_filter->parameters()->strength_min = params.strength_min;
    _peak_filter->parameters()->strength_max = params.strength_max;
    _peak_filter->filter(peaks);
    std::string collection_name = nsx::kw_fitCollection;
    _peak_handler->acceptFilter(collection_name, peaks, listtype::FILTERED);
    PeakCollection* fit_peaks = getPeakCollection(collection_name);

    if (fit_peaks->numberOfPeaks() == 0)
        throw std::runtime_error("buildShapeCollection: no fit peaks found");

    nsxlog(
        Level::Info, "Experiment::buildShapeCollection: ", fit_peaks->numberOfPeaks(), " / ",
        peaks->numberOfPeaks(), " fit peaks");

    nsx::AABB aabb;

    if (params.kabsch_coords) {
        const Eigen::Vector3d sigma(peaks->sigmaD(), peaks->sigmaD(), peaks->sigmaM());
        aabb.setLower(-params.peak_end * sigma);
        aabb.setUpper(params.peak_end * sigma);
    } else {
        const Eigen::Vector3d dx(params.nbins_x, params.nbins_y, params.nbins_z);
        aabb.setLower(-0.5 * dx);
        aabb.setUpper(0.5 * dx);
    }

    ShapeCollection shape_collection =
        ShapeCollection(!params.kabsch_coords, params.peak_end, params.bkg_begin, params.bkg_end);

    std::vector<Peak3D*> fit_peak_list = fit_peaks->getPeakList();
    shape_collection = _integration_handler->integrateShapeCollection(
        fit_peak_list, data, &shape_collection, aabb, params);

    peaks->setShapeCollection(shape_collection);
    // shape_collection.updateFit(1000); // This does nothing!! - zamaan
}

void Experiment::predictPeaks(
    const std::string& name, sptrDataSet data, UnitCell* cell, PredictionParameters* params)
{
    std::vector<nsx::Peak3D*> predicted_peaks;

    nsxlog(Level::Info, "predictPeaks: predicting peaks for data set ", data->name());

    const std::vector<nsx::Peak3D*> predicted =
        nsx::predictPeaks(data, cell, params);

    for (nsx::Peak3D* peak : predicted)
        predicted_peaks.push_back(peak);

    nsxlog(
        Level::Info, "predictPeaks: completed peak prediciton. Added ", predicted_peaks.size(),
        " peaks");

    addPeakCollection(name, listtype::PREDICTED, predicted_peaks);
}

const UnitCell* Experiment::getAcceptedCell() const
{
    return getUnitCell(nsx::kw_acceptedUnitcell);
}

const UnitCell* Experiment::getReferenceCell() const
{
    return getUnitCell(nsx::kw_referenceUnitcell);
}

bool Experiment::refine(PeakCollection* peaks, UnitCell* cell, DataSet* data, int nbatches)
{
    nsxlog(Level::Info, "Experiment::refine: Refining peak collection ", peaks->name());
    std::vector<Peak3D*> peak_list = peaks->getPeakList();
    InstrumentStateList& states = data->instrumentStates();
    _refiner = std::make_unique<Refiner>(states, cell, peak_list, _cell_handler.get(), nbatches);
    _refiner->parameters()->log(Level::Info);
    bool success = _refiner->refine();
    if (success) {
        nsxlog(Level::Info, "Refinement succeeded");
    } else {
        nsxlog(Level::Info, "Refinement failed");
    }
    return success;
}

void Experiment::updatePredictions(PeakCollection* predicted_peaks)
{
    auto peak_list = predicted_peaks->getPeakList();
    int update = _refiner->updatePredictions(peak_list);
    nsxlog(Level::Info, update, "peaks updated");
}

// Data handler methods

Diffractometer* Experiment::getDiffractometer()
{
    return _data_handler->getDiffractometer();
}

const Diffractometer* Experiment::getDiffractometer() const
{
    return _data_handler->getDiffractometer();
}

void Experiment::setDiffractometer(const std::string& diffractometerName)
{
    _data_handler->setDiffractometer(diffractometerName);
}

const DataMap* Experiment::getDataMap() const
{
    return _data_handler->getDataMap();
}

sptrDataSet Experiment::getData(const std::string& name) const
{
    return _data_handler->getData(name);
}

DataList Experiment::getAllData()
{
    return _data_handler->getAllData();
}

int Experiment::numData() const
{
    return _data_handler->numData();
}

void Experiment::addData(sptrDataSet data, std::string name)
{
    _data_handler->addData(data, name);
    setDefaultDMin();
}

bool Experiment::hasData(const std::string& name) const
{
    return _data_handler->hasData(name);
}

void Experiment::removeData(const std::string& name)
{
    _data_handler->removeData(name);
}

// Peak handler methods
void Experiment::addPeakCollection(
    const std::string& name, const listtype type, std::vector<Peak3D*> peaks)
{
    _peak_handler->addPeakCollection(name, type, peaks);
}

bool Experiment::hasPeakCollection(const std::string& name)
{
    return _peak_handler->hasPeakCollection(name);
}

PeakCollection* Experiment::getPeakCollection(const std::string name)
{
    return _peak_handler->getPeakCollection(name);
}

void Experiment::removePeakCollection(const std::string& name)
{
    _peak_handler->removePeakCollection(name);
}

std::vector<std::string> Experiment::getCollectionNames() const
{
    return _peak_handler->getCollectionNames();
}

std::vector<std::string> Experiment::getCollectionNames(listtype lt) const
{
    return _peak_handler->getCollectionNames(lt);
}

int Experiment::numPeakCollections() const
{
    return _peak_handler->numPeakCollections();
}

void Experiment::acceptFilter(std::string name, PeakCollection* collection, listtype lt)
{
    _peak_handler->acceptFilter(name, collection, lt);
}

void Experiment::checkPeakCollections()
{
    _peak_handler->checkPeakCollections();
}

void Experiment::clonePeakCollection(std::string name, std::string new_name)
{
    _peak_handler->clonePeakCollection(name, new_name);
}

// Unit cell handler methods
void Experiment::addUnitCell(const std::string& name, const UnitCell& unit_cell)
{
    _cell_handler->addUnitCell(name, unit_cell);
}

void Experiment::addUnitCell(
    const std::string& name, double a, double b, double c, double alpha, double beta, double gamma)
{
    _cell_handler->addUnitCell(name, a, b, c, alpha, beta, gamma);
}

void Experiment::addUnitCell(
    const std::string& name, double a, double b, double c, double alpha, double beta, double gamma,
    const std::string& space_group)
{
    _cell_handler->addUnitCell(name, a, b, c, alpha, beta, gamma, space_group);
}

bool Experiment::hasUnitCell(const std::string& name) const
{
    return _cell_handler->hasUnitCell(name);
}

std::vector<std::string> Experiment::getUnitCellNames() const
{
    return _cell_handler->getUnitCellNames();
}

UnitCell* Experiment::getUnitCell(const std::string& name) const
{
    return _cell_handler->getUnitCell(name);
}

void Experiment::removeUnitCell(const std::string& name)
{
    _cell_handler->removeUnitCell(name);
}

void Experiment::swapUnitCells(const std::string& old_cell, const std::string& new_cell)
{
    _cell_handler->swapUnitCells(old_cell, new_cell, _peak_handler.get());
}

int Experiment::numUnitCells() const
{
    return _cell_handler->numUnitCells();
}

bool Experiment::checkAndAssignUnitCell(PeakCollection* peaks, double length_tol, double angle_tol)
{
    return _cell_handler->checkAndAssignUnitCell(peaks, _auto_indexer.get(), length_tol, angle_tol);
}

void Experiment::assignUnitCell(PeakCollection* peaks, std::string name)
{
    _cell_handler->assignUnitCell(peaks, name);
}

void Experiment::setReferenceCell(
    double a, double b, double c, double alpha, double beta, double gamma)
{
    _cell_handler->setReferenceCell(a, b, c, alpha, beta, gamma);
}

std::vector<std::string> Experiment::getCompatibleSpaceGroups() const
{
    return _cell_handler->getCompatibleSpaceGroups();
}

UnitCellHandler* Experiment::getCellHandler() const
{
    return _cell_handler.get();
}

// Integration handler methods
IPeakIntegrator* Experiment::getIntegrator(const IntegratorType integrato_type) const
{
    return _integration_handler->getIntegrator(integrato_type);
}

void Experiment::integratePeaks(
    const IntegratorType integrator_type, sptrDataSet data, PeakCollection* peak_collection)
{
    _integration_handler->integratePeaks(integrator_type, data, peak_collection);
}

void Experiment::integratePeaks(
    IPeakIntegrator* integrator, sptrDataSet data, PeakCollection* peaks,
    IntegrationParameters* params, ShapeCollection* shapes)
{
    _integration_handler->integratePeaks(integrator, data, peaks, params, shapes);
}

void Experiment::integrateFoundPeaks()
{
    _integration_handler->integrateFoundPeaks(_peak_finder.get());
}

PeakFinderParameters* Experiment::finderParams()
{
    return _finder_params.get();
}

PeakFilterParameters* Experiment::filterParams()
{
    return _filter_params.get();
}

IndexerParameters* Experiment::indexerParams()
{
    return _indexer_params.get();
}

IntegrationParameters* Experiment::integrationParams()
{
    return _int_params.get();
}

ShapeCollectionParameters* Experiment::shapeParams()
{
    return _shape_params.get();
}

PredictionParameters* Experiment::predictParams()
{
    return _predict_params.get();
}

RefinerParameters* Experiment::refinerParams()
{
    return _refiner_params.get();
}

MergeParameters* Experiment::mergeParams()
{
    return _merge_params.get();
}

} // namespace nsx
