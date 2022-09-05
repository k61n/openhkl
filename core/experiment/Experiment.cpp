//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
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

#include <cstdio> // rename
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>
#include <utility>

#include "base/utils/Logger.h"
#include "base/utils/Path.h" // tempFilename
#include "base/utils/Units.h"
#include "core/data/DataSet.h"
#include "core/experiment/DataHandler.h"
#include "core/experiment/Experiment.h"
#include "core/experiment/ExperimentExporter.h"
#include "core/experiment/ExperimentImporter.h"
#include "core/experiment/InstrumentStateHandler.h"
#include "core/experiment/PeakHandler.h"
#include "core/experiment/ShapeHandler.h"
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
#include "core/loader/IDataReader.h"
#include "core/peak/PeakCoordinateSystem.h"
#include "core/raw/DataKeys.h"
#include "core/raw/MetaData.h"
#include "core/statistics/CC.h"
#include "core/statistics/RFactor.h"
#include "manifest.h"
#include "tables/crystal/UnitCell.h"
#include "core/experiment/MtzExporter.h"

namespace ohkl {

Experiment::~Experiment() = default;

Experiment::Experiment(const std::string& name, const std::string& diffractometerName) : _name(name)
{
    // start logging
    Logger::instance().start(ohkl::kw_logFilename, Level::Info);
    ohklLog(Level::Info, "Git branch ", GIT_BRANCH, " / commit hash ", COMMIT_HASH);

    _instrumentstate_handler = std::make_unique<InstrumentStateHandler>();
    _data_handler =
        std::make_shared<DataHandler>(_name, diffractometerName, _instrumentstate_handler.get());
    _peak_handler = std::make_unique<PeakHandler>();
    _shape_handler = std::make_unique<ShapeHandler>();
    _cell_handler = std::make_unique<UnitCellHandler>();

    _peak_finder = std::make_unique<PeakFinder>();
    _peak_filter = std::make_unique<PeakFilter>();
    _auto_indexer = std::make_unique<AutoIndexer>();
    _predictor = std::make_unique<Predictor>();
    _refiner = std::make_unique<Refiner>(_cell_handler.get());
    _integrator = std::make_unique<Integrator>(_data_handler);
    _peak_merger = std::make_unique<PeakMerger>();
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
    _predictor->parameters()->d_min = d_min;
    _auto_indexer->parameters()->d_min = d_min;
    _peak_filter->parameters()->d_min = d_min;
    _peak_merger->parameters()->d_min = d_min;
}

bool Experiment::acceptFoundPeaks(const std::string& name)
{
    std::vector<Peak3D*> peaks = _peak_finder->currentPeaks();
    return addPeakCollection(name, PeakCollectionType::FOUND, peaks);
}

bool Experiment::acceptFoundPeaks(const std::string& name, const PeakCollection& found)
{
    std::vector<Peak3D*> peaks = found.getPeakList();

    if (!addPeakCollection(
            name, PeakCollectionType::FOUND, peaks, found.isIndexed(),
            _peak_finder->isIntegrated())) {
        return false;
    }
    _peak_finder->setIntegrated(false); // reset for next use
    return true;
}

void Experiment::saveToFile(const std::string& path) const
{
    ohkl::ExperimentExporter exporter;
    ohklLog(Level::Info, "Saving experiment to file: '" + path + "'");

    /* If the chosen path for saving is the same as the path of
       the current dataset file, then a two-step process is used
       to avoid HDF5 errors:
       1. Create a temporary file to store the data.
       2. After writing is finished, rename the temporary file
          to the original given path.
    */

    bool overwrite_datafile = false;
    for (const auto& [ds_nm, ds_ptr] : *_data_handler->getDataMap()) {
        const std::string ohkl_filepath = ds_ptr->reader()->OHKLfilepath();
        if (ohkl_filepath == path) {
            overwrite_datafile = true;
            break;
        }
    }

    std::string filepath{path};
    if (overwrite_datafile) {
        // create a filename for the temporary datafile
        filepath = tempFilename(path);
        ohklLog(Level::Debug, "Saving experiment to temporary file '" + filepath + "'");
    }

    exporter.createFile(name(), getDiffractometer()->name(), filepath);

    std::map<std::string, DataSet*> data_sets;
    for (const auto& it : *_data_handler->getDataMap())
        data_sets.insert(std::make_pair(it.first, it.second.get()));
    exporter.writeData(data_sets);

    std::map<std::string, PeakCollection*> peak_collections;
    for (const auto& it : *_peak_handler->getPeakCollectionMap())
        peak_collections.insert(std::make_pair(it.first, it.second.get()));
    exporter.writePeaks(peak_collections);

    std::vector<UnitCell*> unit_cells;
    for (const auto& cell : *_cell_handler->getCellList())
        unit_cells.push_back(cell.get());
    exporter.writeUnitCells(unit_cells);

    std::map<DataSet*, InstrumentStateSet*> instrument_states;
    for (const auto& it : *_instrumentstate_handler->instrumentStateMap())
        instrument_states.insert({it.first.get(), it.second.get()});
    exporter.writeInstrumentStates(instrument_states);

    exporter.finishWrite();

    if (overwrite_datafile) {
        // rename the temporary datafile to the given filename
        const int rename_success = rename(filepath.c_str(), path.c_str());
        if (rename_success == 0) {
            ohklLog(
                Level::Debug,
                "Renamed the temporary file '" + filepath + "' " + "to '" + path + "'");
        } else {
            ohklLog(
                Level::Error,
                "Could not rename the temporary file '" + filepath + "' to '" + path
                    + "'. Data might be lost.");
        }
    }
}

void Experiment::loadFromFile(const std::string& path)
{
    ohkl::ExperimentImporter importer;
    ohklLog(Level::Info, "Loading experiment from file: '" + path + "'");

    importer.setFilePath(path, this);
    importer.loadData(this);
    importer.loadUnitCells(this);
    importer.loadPeaks(this);
    importer.loadInstrumentStates(this);
    setDefaultDMin();
}

void Experiment::autoIndex(PeakCollection* peaks)
{

    auto params = _auto_indexer->parameters();

    std::string collection_name = ohkl::kw_autoindexingCollection;

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

    ohklLog(
        Level::Info, "Experiment::autoIndex: attempting with frames ", params->first_frame, " - ",
        params->last_frame);
    _peak_filter->filter(peaks);
    double npeaks = peaks->numberOfPeaks();
    double ncaught = peaks->numberCaughtByFilter();
    ohklLog(Level::Info, "Indexing using ", ncaught, " / ", npeaks, " peaks");
    _peak_handler->acceptFilter(collection_name, peaks, PeakCollectionType::INDEXING);
    PeakCollection* indexing_collection = getPeakCollection(collection_name);
    _auto_indexer->autoIndex(indexing_collection);
    _peak_handler->removePeakCollection(collection_name);
}

void Experiment::buildShapeModel(
    PeakCollection* peaks, sptrDataSet data, const ShapeModelParameters& params)
{
    ohklLog(Level::Info, "Experiment::buildShapeModel");
    params.log(Level::Info);
    peaks->computeSigmas();

    _peak_filter->resetFiltering(peaks);
    _peak_filter->resetFilterFlags();
    _peak_filter->flags()->d_range = true;
    _peak_filter->flags()->strength = true;
    _peak_filter->parameters()->d_min = params.d_min;
    _peak_filter->parameters()->d_max = params.d_max;
    _peak_filter->parameters()->strength_min = params.strength_min;
    _peak_filter->parameters()->strength_max = params.strength_max;
    _peak_filter->filter(peaks);
    std::string collection_name = ohkl::kw_fitCollection;
    PeakCollection fit_peaks(collection_name, peaks->type());
    fit_peaks.populateFromFiltered(peaks);

    if (fit_peaks.numberOfPeaks() == 0) {
        ohklLog(Level::Info, "Experiment::buildShapeModel: no fit peaks found");
        return;
    }

    ohklLog(
        Level::Info, "Experiment::buildShapeModel: ", fit_peaks.numberOfPeaks(), " / ",
        peaks->numberOfPeaks(), " fit peaks");

    ohkl::AABB aabb;

    if (params.kabsch_coords) {
        const Eigen::Vector3d sigma(peaks->sigmaD(), peaks->sigmaD(), peaks->sigmaM());
        aabb.setLower(-params.peak_end * sigma);
        aabb.setUpper(params.peak_end * sigma);
    } else {
        const Eigen::Vector3d dx(params.nbins_x, params.nbins_y, params.nbins_z);
        aabb.setLower(-0.5 * dx);
        aabb.setUpper(0.5 * dx);
    }

    std::unique_ptr<ShapeModel> shapes = std::make_unique<ShapeModel>();

    std::vector<Peak3D*> fit_peak_list = fit_peaks.getPeakList();
    _integrator->integrateShapeModel(fit_peak_list, data, shapes.get(), aabb, params);
    peaks->setShapeModel(shapes);

    // shape_model.updateFit(1000); // This does nothing!! - zamaan
    ohklLog(Level::Info, "Experiment::buildShapeModel finished");
}

const UnitCell* Experiment::getAcceptedCell() const
{
    return getUnitCell(ohkl::kw_acceptedUnitcell);
}

const UnitCell* Experiment::getReferenceCell() const
{
    return getUnitCell(ohkl::kw_referenceUnitcell);
}

bool Experiment::refine(
    const PeakCollection* peaks, DataSet* data, sptrUnitCell cell /* = nullptr */)
{
    ohklLog(Level::Info, "Experiment::refine: Refining peak collection ", peaks->name());
    std::vector<Peak3D*> peak_list = peaks->getPeakList();
    _refiner->makeBatches(getInstrumentStateSet(data)->instrumentStates(), peak_list, cell);
    bool success = _refiner->refine();
    if (success) {
        ohklLog(Level::Info, "Refinement succeeded");
    } else {
        ohklLog(Level::Info, "Refinement failed");
    }
    return success;
}

void Experiment::updatePredictions(PeakCollection* predicted_peaks)
{
    auto peak_list = predicted_peaks->getPeakList();
    int update = _refiner->updatePredictions(peak_list);
    ohklLog(Level::Info, update, " peaks updated");
}

Integrator* Experiment::integrator()
{
    return _integrator.get();
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

bool Experiment::addData(sptrDataSet data, bool default_states)
{
    if (!_data_handler->addData(data, data->name()), default_states) {
        return false;
    }
    setDefaultDMin();
    return true;
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
bool Experiment::addPeakCollection(
    const std::string& name, const PeakCollectionType type, std::vector<Peak3D*> peaks)
{
    if (!_peak_handler->hasPeakCollection(name)) {
        _peak_handler->addPeakCollection(name, type, peaks);
        return true;
    }
    return false;
}

bool Experiment::addPeakCollection(
    const std::string& name, const PeakCollectionType type, std::vector<Peak3D*> peaks,
    bool indexed, bool integrated)
{
    return _peak_handler->addPeakCollection(name, type, peaks, indexed, integrated);
}

bool Experiment::hasPeakCollection(const std::string& name)
{
    return _peak_handler->hasPeakCollection(name);
}

bool Experiment::hasPeakCollectionType(PeakCollectionType t)
{
    return _peak_handler->hasPeakCollectionType(t);
}

bool Experiment::hasIntegratedPeakCollection()
{
    return _peak_handler->hasIntegratedPeakCollection();
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

std::vector<std::string> Experiment::getCollectionNames(PeakCollectionType lt) const
{
    return _peak_handler->getCollectionNames(lt);
}

int Experiment::numPeakCollections() const
{
    return _peak_handler->numPeakCollections();
}

bool Experiment::acceptFilter(std::string name, PeakCollection* collection, PeakCollectionType pct)
{
    return _peak_handler->acceptFilter(name, collection, pct);
}

bool Experiment::clonePeakCollection(std::string name, std::string new_name)
{
    return _peak_handler->clonePeakCollection(name, new_name);
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

sptrUnitCell Experiment::getSptrUnitCell(const std::string& name) const
{
    return _cell_handler->getSptrUnitCell(name);
}

sptrUnitCell Experiment::getSptrUnitCell(const unsigned int id) const
{
    return _cell_handler->getSptrUnitCell(id);
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

bool Experiment::checkAndAssignUnitCell(
    PeakCollection* peaks, double length_tol, double angle_tol, std::string name)
{
    return _cell_handler->checkAndAssignUnitCell(
        peaks, _auto_indexer.get(), length_tol, angle_tol, name);
}

void Experiment::assignUnitCell(PeakCollection* peaks, std::string name)
{
    _cell_handler->assignUnitCell(peaks, name);
    peaks->setIndexed(true);
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

bool Experiment::addInstrumentStateSet(sptrDataSet data)
{
    return _instrumentstate_handler->addInstrumentStateSet(data);
}

std::string Experiment::generatePeakCollectionName()
{
    return _peak_handler->generateName();
}

bool Experiment::addInstrumentStateSet(
    sptrDataSet data, const InstrumentStateList& states, bool overwrite)
{
    return _instrumentstate_handler->addInstrumentStateSet(data, states, overwrite);
}

bool Experiment::addInstrumentStateSet(
    sptrDataSet data, std::unique_ptr<InstrumentStateSet>& states)
{
    return _instrumentstate_handler->addInstrumentStateSet(data, states);
}

std::string Experiment::generateUnitCellName()
{
    return _cell_handler->generateUnitCellName();
}

InstrumentStateSet* Experiment::getInstrumentStateSet(const sptrDataSet& data)
{
    return _instrumentstate_handler->getInstrumentStateSet(data);
}

InstrumentStateSet* Experiment::getInstrumentStateSet(const DataSet* data)
{
    return _instrumentstate_handler->getInstrumentStateSet(data);
}

void Experiment::removeInstrumentStateSet(const sptrDataSet& data)
{
    _instrumentstate_handler->removeInstrumentStateSet(data);
}

int Experiment::numInstrumentStateSets() const
{
    return _instrumentstate_handler->numInstrumentStateSets();
}

void Experiment::setLastUnitCellIndex(unsigned int index)
{
    _cell_handler->setLastIndex(index);
}

std::vector<UnitCell*> Experiment::getUnitCells()
{
    return _cell_handler->getUnitCells();
}

std::vector<sptrUnitCell> Experiment::getSptrUnitCells()
{
    return _cell_handler->getSptrUnitCells();
}

std::vector<PeakCollection*> Experiment::getPeakCollections()
{
    return _peak_handler->getPeakCollections();
}

bool Experiment::addShapeModel(const std::string& name, const ShapeModel& shapes)
{
    return _shape_handler->addShapeModel(name, shapes);
}

bool Experiment::addEmptyShapeModel(const std::string& name)
{
    return _shape_handler->addEmptyModel(name);
}

bool Experiment::hasShapeModel(const std::string& name) const
{
    return _shape_handler->hasShapeModel(name);
}

ShapeModel* Experiment::getShapeModel(const std::string name)
{
    return _shape_handler->getShapeModel(name);
}

void Experiment::removeShapeModel(const std::string& name)
{
    _shape_handler->removeShapeModel(name);
}

int Experiment::numShapeModels() const
{
    return _shape_handler->numShapeModels();
}

std::string Experiment::generateShapeModelName()
{
    return _shape_handler->generateName();
}

std::vector<ShapeModel*> Experiment::getShapeModels()
{
    return _shape_handler->getShapeModels();
}

bool Experiment::exportMtz(std::string filename, std::string dataset_name, std::string peak_collection, bool use_merged_data, std::string comment, ohkl::MergedData* merged_data)
{
    /*if (dataset_name.empty())
        dataset_name = this->getAllData()[0]->name();*/

    if (!merged_data) return false;

    MtzExporter exporter(this, dataset_name, peak_collection, use_merged_data, comment, merged_data);
    exporter.exportToFile(filename);
    return true;
}
} // namespace ohkl
