//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/experiment/Experiment.cpp
//! @brief     Implements class Experiment
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "core/experiment/Experiment.h"

#include "base/utils/Logger.h"
#include "base/utils/Path.h" // tempFilename
#include "core/algo/AutoIndexer.h"
#include "core/algo/Refiner.h"
#include "core/data/DataSet.h"
#include "core/experiment/DataHandler.h"
#include "core/experiment/ExperimentExporter.h"
#include "core/experiment/ExperimentImporter.h"
#include "core/experiment/ExperimentYAML.h"
#include "core/experiment/InstrumentStateHandler.h"
#include "core/experiment/Integrator.h"
#include "core/experiment/PeakFinder.h"
#include "core/experiment/PeakFinder2D.h"
#include "core/experiment/PeakHandler.h"
#include "core/experiment/ShapeHandler.h"
#include "core/experiment/UnitCellHandler.h"
#include "core/instrument/Diffractometer.h"
#include "core/instrument/InstrumentStateSet.h"
#include "core/instrument/Monochromator.h"
#include "core/raw/DataKeys.h"
#include "core/raw/MetaData.h"
#include "core/shape/PeakCollection.h"
#include "core/shape/PeakFilter.h"
#include "core/shape/Predictor.h"
#include "core/shape/ShapeModel.h"
#include "core/statistics/MergedPeakCollection.h"
#include "core/statistics/PeakMerger.h"
#include "manifest.h"
#include "tables/crystal/UnitCell.h"

#include <stdexcept>

namespace ohkl {

Experiment::Experiment() : _diffractometer(nullptr), _strategy(false)
{
    // start logging
    Logger::instance().start(ohkl::kw_logFilename, Level::Info);
    ohklLog(Level::Info, "Git branch ", GIT_BRANCH, " / commit hash ", COMMIT_HASH);


    _instrumentstate_handler = std::make_unique<InstrumentStateHandler>();
    _data_handler =
        std::make_shared<DataHandler>(_name, _instrumentstate_handler.get());
    _peak_handler = std::make_unique<PeakHandler>();
    _shape_handler = std::make_unique<ShapeHandler>();
    _cell_handler = std::make_unique<UnitCellHandler>();

    _peak_finder = std::make_unique<PeakFinder>();
    _peak_finder_2d = std::make_unique<PeakFinder2D>();
    _peak_filter = std::make_unique<PeakFilter>();
    _auto_indexer = std::make_unique<AutoIndexer>();
    _predictor = std::make_unique<Predictor>();
    _refiner = std::make_unique<Refiner>(_cell_handler.get());
    _integrator = std::make_unique<Integrator>(_data_handler);
    _peak_merger = std::make_unique<PeakMerger>();

    _shape_params = std::make_shared<ShapeModelParameters>();
}

Experiment::~Experiment() = default;

Experiment::Experiment(const std::string& name, const std::string& diffractometerName) : Experiment()
{
    _name = name;
    setDiffractometer(diffractometerName);
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

void Experiment::readFromYaml(const std::string& filename)
{
    ohklLog(Level::Info, "Experiment::readFromYaml:  ", filename);
    ExperimentYAML yaml(filename);
    yaml.grabPeakFinderParameters(_peak_finder->parameters());
    yaml.grabAutoindexerParameters(_auto_indexer->parameters());
    yaml.grabShapeParameters(_shape_params.get());
    yaml.grabPredictorParameters(_predictor->parameters());
    yaml.grabIntegrationParameters(_integrator->parameters());
    yaml.grabMergeParameters(_peak_merger->parameters());
}

void Experiment::saveToYaml(const std::string& filename)
{
    ohklLog(Level::Info, "Experiment::saveToYaml:  ", filename);
    ExperimentYAML yaml(filename);
    yaml.setPeakFinderParameters(_peak_finder->parameters());
    yaml.setAutoindexerParameters(_auto_indexer->parameters());
    yaml.setShapeParameters(_shape_params.get());
    yaml.setPredictorParameters(_predictor->parameters());
    yaml.setIntegrationParameters(_integrator->parameters());
    yaml.setMergeParameters(_peak_merger->parameters());
    yaml.writeFile(filename);
}

Diffractometer* Experiment::getDiffractometer()
{
    return _diffractometer.get();
}

void Experiment::setDiffractometer(const std::string& diffractometerName)
{
    _diffractometer.reset(Diffractometer::create(diffractometerName));
}

bool Experiment::acceptFoundPeaks(const std::string& name)
{
    std::vector<Peak3D*> peaks = _peak_finder->currentPeaks();
    sptrDataSet data = _peak_finder->currentData();
    return addPeakCollection(name, PeakCollectionType::FOUND, peaks, data, nullptr);
}

bool Experiment::acceptFoundPeaks(const std::string& name, const PeakCollection& found)
{
    std::vector<Peak3D*> peaks = found.getPeakList();

    if (!addPeakCollection(
            name, PeakCollectionType::FOUND, peaks, _peak_finder->currentData(), nullptr,
            found.isIndexed(), _peak_finder->isIntegrated(), _peak_finder->hasBkgGradient())) {
        return false;
    }
    _peak_finder->setIntegrated(false); // reset for next use
    _peak_finder->setBkgGradient(false);
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

    exporter.createFile(name(), _diffractometer->name(), filepath, _strategy);

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

const UnitCell* Experiment::getAcceptedCell() const
{
    return getUnitCell(ohkl::kw_acceptedUnitcell);
}

const UnitCell* Experiment::getReferenceCell() const
{
    return getUnitCell(ohkl::kw_referenceUnitcell);
}

Integrator* Experiment::integrator()
{
    return _integrator.get();
}

// Data handler methods

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
    if (!_data_handler->addData(data, _diffractometer.get(), data->name(), default_states)) {
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
    const std::string& name, const PeakCollectionType type, std::vector<Peak3D*> peaks,
    sptrDataSet data, sptrUnitCell cell)
{
    if (!_peak_handler->hasPeakCollection(name)) {
        _peak_handler->addPeakCollection(name, type, peaks, data, cell);
        return true;
    }
    return false;
}

bool Experiment::addPeakCollection(
    const std::string& name, const PeakCollectionType type, std::vector<Peak3D*> peaks,
    sptrDataSet data, sptrUnitCell cell, bool indexed, bool integrated, bool gradient)
{
    return _peak_handler->addPeakCollection(
        name, type, peaks, data, cell, indexed, integrated, gradient);
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

int Experiment::numPeakCollections() const
{
    return _peak_handler->numPeakCollections();
}

bool Experiment::acceptFilter(
    std::string name, PeakCollection* collection, PeakCollectionType pct, sptrDataSet data)
{
    return _peak_handler->acceptFilter(name, collection, pct, data);
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
    const std::string& name, double a, double b, double c, double alpha, double beta, double gamma,
    sptrDataSet data)
{
    _cell_handler->addUnitCell(name, a, b, c, alpha, beta, gamma, data);
}

void Experiment::addUnitCell(
    const std::string& name, double a, double b, double c, double alpha, double beta, double gamma,
    const std::string& space_group, sptrDataSet data)
{
    _cell_handler->addUnitCell(name, a, b, c, alpha, beta, gamma, space_group, data);
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
    double a, double b, double c, double alpha, double beta, double gamma, sptrDataSet data)
{
    _cell_handler->setReferenceCell(a, b, c, alpha, beta, gamma, data);
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
    return _cell_handler->generateName();
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

std::vector<sptrUnitCell> Experiment::getSptrUnitCells(sptrDataSet data /* = nullptr */)
{
    if (data)
        return _cell_handler->getSptrUnitCells(data);
    else
        return _cell_handler->getSptrUnitCells();
}

std::vector<PeakCollection*> Experiment::getPeakCollections(sptrDataSet data)
{
    if (data)
        return _peak_handler->getPeakCollections(data);
    else
        return _peak_handler->getPeakCollections();
}

bool Experiment::addShapeModel(const std::string& name, const ShapeModel& shapes)
{
    return _shape_handler->addShapeModel(name, shapes);
}

bool Experiment::addShapeModel(const std::string& name, std::unique_ptr<ShapeModel>& shapes)
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

std::vector<ShapeModel*> Experiment::getShapeModels(sptrDataSet data)
{
    if (data)
        return _shape_handler->getShapeModels(data);
    else
        return _shape_handler->getShapeModels();
}

} // namespace ohkl
