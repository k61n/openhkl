//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/experiment/Experiment.h
//! @brief     Defines class Experiment
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_CORE_EXPERIMENT_EXPERIMENT_H
#define OHKL_CORE_EXPERIMENT_EXPERIMENT_H

#include "core/data/DataTypes.h"
#include "core/instrument/InstrumentState.h"
#include "core/raw/DataKeys.h"

#include <map>
#include <memory>

namespace ohkl {

enum class PeakCollectionType;
class AutoIndexer;
class DataHandler;
class Diffractometer;
class InstrumentStateSet;
class InstrumentStateHandler;
class Integrator;
class Peak3D;
class PeakCollection;
class PeakFilter;
class PeakFinder;
class PeakFinder2D;
class PeakHandler;
class PeakMerger;
class Predictor;
class Refiner;
class ShapeHandler;
class ShapeModel;
class ShapeModelBuilder;
class UnitCellHandler;
struct DataReaderParameters;
struct ShapeModelParameters;

using DataMap = std::map<std::string, sptrDataSet>;

/*! \addtogroup python_api
 *  @{*/

/*! \brief Top level core object wrapping all other core objects
 *
 *  The Experiment object contains organises and controls almost all other core
 *  classes. It is a container for DataSets, PeakCollections and UnitCells.
 *  These objects are managed via their handlers, DataHandler, PeakHandler and
 *  CellHandler. It also calls the principal algorithms for finding peaks,
 *  integration, autoindexing, refining and merging peak collections.
 */
class Experiment {
 public:
    Experiment();
    Experiment(const std::string& name, const std::string& diffractometerName);
    ~Experiment();

    Experiment(const Experiment& other) = delete;

    //! Get the name of the Experiment
    const std::string& name() const;
    //! Set the name of the Experiment
    void setName(const std::string& name);

    //! Read saved experiment parameters from yaml
    void readFromYaml(const std::string& filename);
    //! Save the parameters of the experiment to yaml
    void saveToYaml(const std::string& filename);

    //! Get a pointer to the diffractometer
    Diffractometer* getDiffractometer(); 
    //! Set the named diffractometer from the relevant YAML instrument file
    void setDiffractometer(const std::string& diffractometerName);

    //! Get a pointer to the data reader parameters
    DataReaderParameters* dataReaderParameters() const {return _data_reader_params.get(); };

    // Data handler
    //! Get the map of data sets from the handler
    const DataMap* getDataMap() const;
    //! Get shared pointer to named data set
    sptrDataSet getData(const std::string& name) const;
    //! Get a vector containing shared pointers to all data sets
    DataList getAllData();
    //! Return number of data sets
    int numData() const;
    //! Add a data set to the handler
    bool addData(sptrDataSet data, bool default_states = true);
    //! Check whether the handler has named data set
    bool hasData(const std::string& name) const;
    //! Remove data set from handler
    void removeData(const std::string& name);

    // Peak handler
    //! Create a new PeakCollection from a vector of peaks
    bool addPeakCollection(
        const std::string& name, const PeakCollectionType type, std::vector<Peak3D*> peaks,
        sptrDataSet data, sptrUnitCell cell);
    bool addPeakCollection(
        const std::string& name, const PeakCollectionType type, std::vector<Peak3D*> peaks,
        sptrDataSet data, sptrUnitCell cell, bool indexed, bool integrated, bool gradient);
    //! Check if the handler has the named peak collection
    bool hasPeakCollection(const std::string& name);
    //! Check if handler has Peak Collections of a certain type
    bool hasPeakCollectionType(PeakCollectionType t);
    //! returns whether or not experiment has integrated peakcollections
    bool hasIntegratedPeakCollection();
    //! Get a pointer to the named peak collection
    PeakCollection* getPeakCollection(const std::string name);
    //! Remove the named peak collection
    void removePeakCollection(const std::string& name);
    //! Get the number of peak collections
    int numPeakCollections() const;
    //! Create a new peak collection from peaks caught by a filter
    bool acceptFilter(
        std::string name, PeakCollection* collection, PeakCollectionType pct, sptrDataSet data);
    //! Duplicate a peak collection (deep copy) for comparison after some process
    bool clonePeakCollection(std::string name, std::string new_name);

    // Unit cells
    //! Add a unit cell to the experiment
    void addUnitCell(const std::string& name, const UnitCell& unit_cell);
    //! Add a unit cell to the experiment via cell parameters (skip autoindexing step)
    void addUnitCell(
        const std::string& name, double a, double b, double c, double alpha, double beta,
        double gamma, sptrDataSet data);
    //! Add a user-defined unit cell including space group
    void addUnitCell(
        const std::string& name, double a, double b, double c, double alpha, double beta,
        double gamma, const std::string& space_group, sptrDataSet data);
    //! Returns true if the experiment has a data
    bool hasUnitCell(const std::string& name) const;
    //! Get a list of loaded list names
    std::vector<std::string> getUnitCellNames() const;
    //! Return a pointer to the named unit cell
    UnitCell* getUnitCell(const std::string& name) const;
    //! Return a pointer to the named unit cell
    sptrUnitCell getSptrUnitCell(const std::string& name) const;
    //! Return a pointer to the numbered unit cell
    sptrUnitCell getSptrUnitCell(const unsigned int id) const;
    //! Remove a unit cell from the experiment
    void removeUnitCell(const std::string& name);
    //! Swap two unit cells in the map contained by the handler
    void swapUnitCells(const std::string& old_cell, const std::string& new_cell);
    //! Get the number of unit cells in the map
    int numUnitCells() const;
    //! Check solution against reference cell and accept if within tolerances
    bool checkAndAssignUnitCell(
        PeakCollection* peaks, double length_tol, double angle_tol,
        std::string name = kw_acceptedUnitcell);
    //! Assign unit cell to a peak collection, compute Miller indices from q and cell
    void assignUnitCell(PeakCollection* peaks, std::string cellName = ohkl::kw_acceptedUnitcell);
    //! Set the reference cell
    void setReferenceCell(
        double a, double b, double c, double alpha, double beta, double gamma, sptrDataSet data);
    //! Get space groups compatible with unit cell
    std::vector<std::string> getCompatibleSpaceGroups() const;
    //! Get the cell handler
    UnitCellHandler* getCellHandler() const;
    //! set last unit cell index in cell handler
    void setLastUnitCellIndex(unsigned int index);
    //! Get a vector of unit cells in the experiment
    std::vector<UnitCell*> getUnitCells();
    //! Get a vector of unit cells in the experiment
    std::vector<sptrUnitCell> getSptrUnitCells(sptrDataSet data = nullptr);
    //! get a vector of pointers to peak collections
    std::vector<PeakCollection*> getPeakCollections(sptrDataSet data = nullptr);

    // ShapeHandler
    //! Add a shape model
    bool addShapeModel(const std::string& name, const ShapeModel& shapes);
    //! Add a shape model from a unique_ptr
    bool addShapeModel(const std::string& name, std::unique_ptr<ShapeModel>& shapes);
    //! Returns true if the experiment has named shape model
    bool hasShapeModel(const std::string& name) const;
    //! Returns the named shape model
    ShapeModel* getShapeModel(const std::string name);
    //! Remove a shape model from the experiment
    void removeShapeModel(const std::string& name);
    //! Get the number of shape models
    int numShapeModels() const;
    //! Generate name for new shape model
    std::string generateShapeModelName();
    //! Get a vector of pointers to shape models
    std::vector<ShapeModel*> getShapeModels(sptrDataSet data = nullptr);

    // Instrument state handler
    //! Add a set of instrment states
    bool addInstrumentStateSet(sptrDataSet data);
    //! Add a set of instrment states
    bool addInstrumentStateSet(
        sptrDataSet data, const InstrumentStateList& states, bool overwrite = true);
    //! Add a set of instrment states
    bool addInstrumentStateSet(sptrDataSet data, std::unique_ptr<InstrumentStateSet>& states);
    //! Returns the named InstrumentStateSet
    InstrumentStateSet* getInstrumentStateSet(const sptrDataSet& data);
    //! Returns the named InstrumentStateSet
    InstrumentStateSet* getInstrumentStateSet(const DataSet* data);
    //! Remove a set of instrument states from the experiment
    void removeInstrumentStateSet(const sptrDataSet& data);
    //! Return number of instrument state sets
    int numInstrumentStateSets() const;

    // Peak finder
    //! Return a pointer to the PeakFinder object
    PeakFinder* peakFinder() { return _peak_finder.get(); };
    //! Return a pointer to the OpenCV peak finder
    PeakFinder2D* peakFinder2D() { return _peak_finder_2d.get(); };
    //! Create a new peak collection from the peaks found by the peak finder
    bool acceptFoundPeaks(const std::string& name);
    //! Create a new peak collection from a found collection
    bool acceptFoundPeaks(const std::string& name, const PeakCollection& found);

    // Peak Filter
    //! Return a pointer to the PeakFilter object
    PeakFilter* peakFilter() { return _peak_filter.get(); };

    // Autoindexer
    //! Get a pointer to the AutoIndexer object
    AutoIndexer* autoIndexer() const { return _auto_indexer.get(); };
    //! Get a pointer to the accepted/assigned unit cell
    const UnitCell* getAcceptedCell() const;
    //! Get a pointer to the reference unit cell
    const UnitCell* getReferenceCell() const;

    // Shape model builder
    //! Get a pointer to the ShapeModelBuilder object
    ShapeModelBuilder* shapeModelBuilder() const { return _shape_model_builder.get(); };

    // Peak Merger
    //! get a pointer to the PeakMerger object
    PeakMerger* peakMerger() const { return _peak_merger.get(); };

    // Save load
    //! Save the current experiment state to hdf5
    void saveToFile(const std::string& path) const;
    //! Load the current experiment state to hdf5
    void loadFromFile(const std::string& path);

    // Prediction
    //! Get a pointer to the predictor
    Predictor* predictor() { return _predictor.get(); };
    //! Get a pointer to the refiner
    Refiner* refiner() { return _refiner.get(); };

    // Integration
    //! Get a pointer to the integrator module
    Integrator* integrator();

    //! Generate automatic name for PeakCollection
    std::string generatePeakCollectionName();

    //! Generate automatic name for UnitCell
    std::string generateUnitCellName();

    //! Toggle strategy mode
    void setStrategy(bool flag) { _strategy = flag; };
    //! Get strategy mode flag
    bool strategy() const { return _strategy; };

 private: // private variables
    std::string _name; //!< The name of this experiment
    std::unique_ptr<Diffractometer> _diffractometer;

    // Handlers for peak collections and unit cells
    std::shared_ptr<DataHandler> _data_handler; // shared because Integrator needs access
    std::unique_ptr<PeakHandler> _peak_handler;
    std::unique_ptr<ShapeHandler> _shape_handler;
    std::unique_ptr<UnitCellHandler> _cell_handler;
    std::unique_ptr<InstrumentStateHandler> _instrumentstate_handler;

    // Objects that do the number crunching
    std::unique_ptr<PeakFinder> _peak_finder;
    std::unique_ptr<PeakFinder2D> _peak_finder_2d;
    std::unique_ptr<PeakFilter> _peak_filter;
    std::unique_ptr<AutoIndexer> _auto_indexer;
    std::unique_ptr<ShapeModelBuilder> _shape_model_builder;
    std::unique_ptr<Predictor> _predictor;
    std::unique_ptr<Refiner> _refiner;
    std::unique_ptr<Integrator> _integrator;
    std::unique_ptr<PeakMerger> _peak_merger;

    // Data reader parameters, since there is nowhere else to store this
    std::unique_ptr<DataReaderParameters> _data_reader_params;

    bool _strategy;
};

/*! @}*/
} // namespace ohkl

#endif // OHKL_CORE_EXPERIMENT_EXPERIMENT_H
