//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/experiment/Experiment.h
//! @brief     Defines class Experiment
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef NSX_CORE_EXPERIMENT_EXPERIMENT_H
#define NSX_CORE_EXPERIMENT_EXPERIMENT_H

#include "core/algo/AutoIndexer.h"
#include "core/algo/Refiner.h"
#include "core/data/DataTypes.h"
#include "core/experiment/DataQuality.h"
#include "core/experiment/PeakFinder.h"
#include "core/instrument/Diffractometer.h"
#include "core/integration/Integrator.h"
#include "core/loader/RawDataReader.h"
#include "core/raw/DataKeys.h"
#include "core/shape/IPeakIntegrator.h"
#include "core/shape/PeakFilter.h"
#include "core/shape/Predictor.h"
#include "core/statistics/MergedData.h"
#include "core/statistics/PeakMerger.h"
#include "core/statistics/ResolutionShell.h"
#include "tables/crystal/UnitCell.h"


namespace nsx {

class DataHandler;
class PeakHandler;
class UnitCellHandler;

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
    Experiment() = delete;
    Experiment(const std::string& name, const std::string& diffractometerName);
    ~Experiment();

    Experiment(const Experiment& other) = delete;

    //! Get the name of the Experiment
    const std::string& name() const;
    //! Set the name of the Experiment
    void setName(const std::string& name);
    //! Default d min to lambda/2
    void setDefaultDMin();

    // Data handler
    //! Get a pointer to the diffractometer contained in the DataHandler object
    Diffractometer* getDiffractometer();
    //! Get a pointer to the diffractometer contained in the DataHandler object
    const Diffractometer* getDiffractometer() const;
    //! Set the named diffractometer from the relevant YAML instrument file
    void setDiffractometer(const std::string& diffractometerName);
    //! Get the map of data sets from the handler
    const DataMap* getDataMap() const;
    //! Get shared pointer to named data set
    sptrDataSet getData(const std::string& name) const;
    //! Get a vector containing shared pointers to all data sets
    DataList getAllData();
    //! Return number of data sets
    int numData() const;
    //! Add a data set to the handler
    bool addData(sptrDataSet data);
    //! Check whether the handler has named data set
    bool hasData(const std::string& name) const;
    //! Remove data set from handler
    void removeData(const std::string& name);

    // Peak handler
    //! Create a new PeakCollection from a vector of peaks
    bool addPeakCollection(
        const std::string& name, const listtype type, std::vector<Peak3D*> peaks);
    bool addPeakCollection(
        const std::string& name, const listtype type, std::vector<Peak3D*> peaks, bool indexed, bool integrated);
    //! Check if the handler has the named peak collection
    bool hasPeakCollection(const std::string& name);
    //! Get a pointer to the named peak collection
    PeakCollection* getPeakCollection(const std::string name);
    //! Remove the named peak collection
    void removePeakCollection(const std::string& name);
    //! Get a vector of peak collection names
    std::vector<std::string> getCollectionNames() const;
    //! Get a vector of peak collection names of a give listtype
    std::vector<std::string> getCollectionNames(listtype lt) const;
    //! Get the number of peak collections
    int numPeakCollections() const;
    //! Create a new peak collection from peaks caught by a filter
    void acceptFilter(
        std::string name, PeakCollection* collection, listtype lt = listtype::FILTERED);
    //! Duplicate a peak collection (deep copy) for comparison after some process
    void clonePeakCollection(std::string name, std::string new_name);

    // Unit cells
    //! Add a unit cell to the experiment
    bool addUnitCell(const std::string& name, const UnitCell& unit_cell, bool refined = false);
    //! Add a unit cell to the experiment via cell parameters (skip autoindexing step)
    bool addUnitCell(
        const std::string& name, double a, double b, double c, double alpha, double beta,
        double gamma);
    //! Add a user-defined unit cell including space group
    bool addUnitCell(
        const std::string& name, double a, double b, double c, double alpha, double beta,
        double gamma, const std::string& space_group);
    //! Returns true if the experiment has a data
    bool hasUnitCell(const std::string& name) const;
    //! Get a list of loaded list names
    std::vector<std::string> getUnitCellNames() const;
    //! Return a pointer to the named unit cell
    UnitCell* getUnitCell(const std::string& name) const;
    //! Return a pointer to the named unit cell
    sptrUnitCell getSptrUnitCell(const std::string& name) const;
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
    void assignUnitCell(PeakCollection* peaks, std::string cellName = nsx::kw_acceptedUnitcell);
    //! Set the reference cell
    void setReferenceCell(double a, double b, double c, double alpha, double beta, double gamma);
    //! Get space groups compatible with unit cell
    std::vector<std::string> getCompatibleSpaceGroups() const;
    //! Get the cell handler
    UnitCellHandler* getCellHandler() const;
    //! Remove the batch cells if they are not being used
    void removeBatchCells();

    // Peak finder
    //! Return a pointer to the PeakFinder object
    PeakFinder* peakFinder() { return _peak_finder.get(); };
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
    //! attempt to autoindex the data
    void autoIndex(PeakCollection* peaks);
    //! Get a pointer to the accepted/assigned unit cell
    const UnitCell* getAcceptedCell() const;
    //! Get a pointer to the reference unit cell
    const UnitCell* getReferenceCell() const;

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
    //! Construct the collection used to fit the shapes of predicted peaks
    void buildShapeCollection(
        PeakCollection* peaks, sptrDataSet data, const ShapeCollectionParameters& params);
    //! Get a pointer to the refiner
    Refiner* refiner() { return _refiner.get(); };
    //! Refine unit cell and instrument parameters
    bool refine(const PeakCollection* peaks, DataSet* data, sptrUnitCell cell = nullptr);
    //! Update the predicted peaks post-refinement
    void updatePredictions(PeakCollection* predicted_peaks);

    // Integration
    //! Get a pointer to the integrator module
    Integrator* integrator();

    //! Return data quality resolution
    DataResolution* getResolution() { return &_data_resolution; };
    // Return data quality structs for all merged data:
    DataResolution* getQuality() { return &_data_quality; };

    //! Container for metadata for reading raw data files
    RawDataReaderParameters data_params;

    // auto generating names for collections
    std::string GeneratePeakCollectionName();
    std::string GenerateUnitCellName();

 private: // private variables
    std::string _name; //!< The name of this experiment

    // Handlers for peak collections and unit cells
    std::shared_ptr<DataHandler> _data_handler; // shared because Integrator needs access
    std::unique_ptr<PeakHandler> _peak_handler;
    std::unique_ptr<UnitCellHandler> _cell_handler;

    // Objects that do the number crunching
    std::unique_ptr<PeakFinder> _peak_finder;
    std::unique_ptr<PeakFilter> _peak_filter;
    std::unique_ptr<AutoIndexer> _auto_indexer;
    std::unique_ptr<Predictor> _predictor;
    std::unique_ptr<Refiner> _refiner;
    std::unique_ptr<Integrator> _integrator;
    std::unique_ptr<PeakMerger> _peak_merger;

    // Objects containing quality metrics
    DataResolution _data_quality; //!< Data quality for whole resolution range
    DataResolution _data_resolution; //!< Data quality per resolution shell
};

/*! @}*/
} // namespace nsx

#endif // NSX_CORE_EXPERIMENT_EXPERIMENT_H
