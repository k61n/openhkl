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
#include "core/loader/RawDataReader.h"
#include "core/shape/IPeakIntegrator.h"
#include "core/shape/PeakFilter.h"
#include "core/statistics/MergedData.h"
#include "core/statistics/ResolutionShell.h"
#include "tables/crystal/UnitCell.h"

namespace nsx {

class DataHandler;
class PeakHandler;
class UnitCellHandler;
class IntegrationHandler;

using DataMap = std::map<std::string, sptrDataSet>;

//! Experiment class, a data type containing a diffractometer and data sets.
class Experiment {
 public:
    Experiment() = delete;
    Experiment(const std::string& name, const std::string& diffractometerName);
    ~Experiment();

    Experiment(const Experiment& other) = delete;

    // General
    const std::string& name() const;
    void setName(const std::string& name);

    // Data handler
    //! Get a pointer to the diffractometer contained in the DataHandler object
    Diffractometer* getDiffractometer();
    const Diffractometer* getDiffractometer() const;
    //! Set the named diffractometer from the instrument file
    void setDiffractometer(const std::string& diffractometerName);
    //! Get maps of data sets from handler
    const DataMap* getDataMap() const;
    //! Get shared pointer to named data set
    sptrDataSet getData(const std::string& name) const;
    //! Get a vector containing shared pointers to all data sets
    DataList getAllData();
    sptrDataSet dataShortName(const std::string& name);
    //! Return number of data sets
    int numData() const;
    //! Add a data set to the handler
    void addData(sptrDataSet data, std::string name = "");
    //! Check whether the handler has named data set
    bool hasData(const std::string& name) const;
    //! Remove data set from handler
    void removeData(const std::string& name);

    // Peak handler
    //! Create a new PeakCollection from a vector of peaks
    void addPeakCollection(
        const std::string& name, const listtype type, std::vector<Peak3D*> peaks);
    //! Check if the handler has the named peak collection
    bool hasPeakCollection(const std::string& name);
    //! Get a pointer to the named peak collection
    PeakCollection* getPeakCollection(const std::string name);
    //! Remove the named peak collection
    void removePeakCollection(const std::string& name);
    //! Get a vector of peak collection names
    std::vector<std::string> getCollectionNames() const;
    //! Get a vector of names of peak collections of listtype::FOUND
    std::vector<std::string> getFoundCollectionNames() const;
    //! Get a vector of names of peak collections of listtype::PREDICTD
    std::vector<std::string> getPredictedCollectionNames() const;
    //! Get the number of peak collections
    int numPeakCollections() const;
    //! Create a new peak collection from peaks caught by a filter
    void acceptFilter(std::string name, PeakCollection* collection);
    //! Check for unphysical peaks in all collections
    void checkPeakCollections();
    //! Merge a vector of PeakCollection objects
    void setMergedPeaks(std::vector<PeakCollection*> peak_collections, bool friedel);
    //! Merge two PeakCollection objects (mainly for SWIG)
    void setMergedPeaks(PeakCollection* found, PeakCollection* predicted, bool friedel);
    //! Reset the merged peak collection
    void resetMergedPeaks();
    //! Get the merged peak collection
    MergedData* getMergedPeaks() const;

    // Unit cells
    //! Add a unit cell to the experiment
    void addUnitCell(const std::string& name, const UnitCell& unit_cell);
    //! Add a unit cell to the experiment via cell parameters (skip autoindexing step)
    void addUnitCell(
        const std::string& name, double a, double b, double c, double alpha, double beta,
        double gamma);
    //! Returns true if the experiment has a data
    bool hasUnitCell(const std::string& name) const;
    //! Get a list of loaded list names
    std::vector<std::string> getUnitCellNames() const;
    //! Return a pointer to the named unit cell
    UnitCell* getUnitCell(const std::string& name) const;
    //! Remove a unit cell from the experiment
    void removeUnitCell(const std::string& name);
    //! Swap two unit cells in the map contained by the handler
    void swapUnitCells(const std::string& old_cell, const std::string& new_cell);
    //! Get the number of unit cells in the map
    int numUnitCells() const;
    //! Check solution against reference cell and accept if within tolerances
    bool checkAndAssignUnitCell(PeakCollection* peaks, double length_tol, double angle_tol);
    //! Assign unit cell to a peak collection, compute Miller indices from q and cell
    void assignUnitCell(PeakCollection* peaks);
    //! Set the reference cell
    void setReferenceCell(double a, double b, double c, double alpha, double beta, double gamma);
    //! Get space groups compatible with unit cell
    std::vector<std::string> getCompatibleSpaceGroups() const;
    //! Get the cell handler
    UnitCellHandler* getCellHandler() const;

    // Peak finder
    //! Return a pointer to the PeakFinder object
    nsx::PeakFinder* peakFinder() { return _peak_finder.get(); };
    //! Create a new peak collection from the peaks found by the peak finder
    void acceptFoundPeaks(const std::string& name);

    // Peak Filter
    //! Return a pointer to the PeakFilter object
    nsx::PeakFilter* peakFilter() { return _peak_filter.get(); };

    // Autoindexer
    //! Get a pointer to the AutoIndexer object
    nsx::AutoIndexer* autoIndexer() const { return _auto_indexer.get(); };
    //! attempt to autoindex the data
    void autoIndex(PeakCollection* peaks, const IndexerParameters& params);
    //! Run the autoindexer over the dataset, vary number of frames in attempt to find cell
    bool runAutoIndexer(
        PeakCollection* peaks, IndexerParameters& params, const double& length_tol,
        const double& angle_tol, const double& frame_min, const double& frame_max);
    //! Get a pointer to the accepted/assigned unit cell
    const UnitCell* getAcceptedCell() const;
    //! Get a pointer to the reference unit cell
    const UnitCell* getReferenceCell() const;

    // Integration handler
    //! get the named integrator
    IPeakIntegrator* getIntegrator(const std::string& name) const;
    //! Integrate the given peak collection with the named integrator
    void integratePeaks(
        const std::string& integrator_name, PeakCollection* peak_collection, double d_min,
        double d_max);
    //! Integrate predicted peaks, fitting shappes from given shape library
    void integratePredictedPeaks(
        const std::string& integrator_name, PeakCollection* peak_collection,
        ShapeLibrary* shape_library, PredictionParameters& params);
    //! Integrate peaks found by _peak_finder
    void integrateFoundPeaks(const std::string& integrator);

    // Save load
    //! Save the current experiment state to hdf5
    void saveToFile(const std::string& path) const;
    //! Load the current experiment state to hdf5
    void loadFromFile(const std::string& path);

    // Prediction
    //! Construct the library used to fit the shapes of predicted peaks
    void buildShapeLibrary(PeakCollection* peaks, const ShapeLibParameters& params);
    //! Predict peaks from unit cell
    void predictPeaks(
        const std::string& name, PeakCollection* peaks, const PredictionParameters& params,
        PeakInterpolation interpol);
    //! Refine unit cell and instrument parameters
    void refine(
        const PeakCollection* peaks, const PeakCollection* predicted_peaks, UnitCell* cell,
        DataSet* data, int n_batches);

    //! Get resolution shells for quality metrics
    void computeQuality(
        double d_min, double d_max, int n_shells, PeakCollection* predicted, PeakCollection* found,
        bool friedel);
    //! Return data quality resolution
    DataResolution* getResolution() { return &_data_resolution; };
    // Return data quality structs for all merged data:
    const DataQuality& getQuality() { return _data_quality; };

    // Objects containing integration parameters. If I construct these on their own in Python,
    // they get destroyed when passed to Experiment, hence the kludgy option of making them
    // public - zamaan
    IntegrationParameters int_params;
    ShapeLibParameters shape_params;
    PredictionParameters predict_params;
    RawDataReaderParameters data_params;
    IndexerParameters indexer_params;

 private: // private variables
    std::string _name; //!< The name of this experiment
    std::unique_ptr<Diffractometer> _diffractometer;

    // Handlers for peak collections and unit cells
    std::shared_ptr<DataHandler> _data_handler; // shared because IntegrationHandler needs access
    std::unique_ptr<PeakHandler> _peak_handler;
    std::unique_ptr<UnitCellHandler> _cell_handler;
    std::unique_ptr<IntegrationHandler> _integration_handler;

    // Objects that do the number crunching
    std::unique_ptr<nsx::PeakFinder> _peak_finder;
    std::unique_ptr<nsx::PeakFilter> _peak_filter;
    std::unique_ptr<nsx::AutoIndexer> _auto_indexer;

    // Objects containing quality metrics
    DataQuality _data_quality;
    DataResolution _data_resolution; //!< Data quality per resolution shell
};

} // namespace nsx

#endif // NSX_CORE_EXPERIMENT_EXPERIMENT_H
