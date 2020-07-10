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

#ifndef CORE_EXPERIMENT_EXPERIMENT_H
#define CORE_EXPERIMENT_EXPERIMENT_H

#include "core/algo/AutoIndexer.h"
#include "core/data/DataTypes.h"
#include "core/experiment/PeakFinder.h"
#include "core/instrument/Diffractometer.h"
#include "core/shape/IPeakIntegrator.h"
#include "core/shape/PeakFilter.h"
#include "core/statistics/CC.h"
#include "core/statistics/MergedData.h"
#include "core/statistics/RFactor.h"
#include "core/statistics/ResolutionShell.h"
#include "tables/crystal/UnitCell.h"

namespace nsx {

//! Experiment class, a data type containing a diffractometer and data sets.

struct DataQuality {
    double Rmerge; //! R-factor
    double expectedRmerge; //! expected R-factor
    double Rmeas; //! multiplicity-weighted R-factor
    double expectedRmeas; //! expected multiplicity-weighted R-factor
    double Rpim; //! relative (precision-indicating) R-factor
    double expectedRpim; //! expected relative R-factor
    double CChalf; //! CC_{1/2} correlation coefficient
    double CCstar; //! estimate of CC_{true} derived from CC_{1/2}
};

struct DataResolution : DataQuality {
    double dmin; //! Lower limit of d for resolution shell
    double dmax; //! Upper limit of d for resolution shell
};

class Experiment {

 public:
    Experiment() = delete;
    Experiment(const Experiment& other);
    //! Construct an empty experiment from a given name and diffractometer
    Experiment(const std::string& name, const std::string& diffractometerName);
    ~Experiment() = default;
    Experiment& operator=(const Experiment& other);

 public: // General
    //! Get the name of the Experiment
    const std::string& name() const;
    //! Sets the name of the experiment
    void setName(const std::string& name);
    //! Returns a non-const pointer to the diffractometer related to the experiment
    Diffractometer* diffractometer();
    //! Returns a const pointer to the diffractometer related to the experiment
    const Diffractometer* diffractometer() const;
    //! Set the diffractometer accordingly
    void setDiffractometer(const std::string& diffractometerName);

 public: // Data sets
    //! Gets a reference to the data
    const std::map<std::string, sptrDataSet>& getData() const;
    //! Gets the pointer to a given data stored in the experiment
    sptrDataSet getData(const std::string& name);
    //! Return all data sets as a DataList
    DataList getAllData();
    //! Gets the pointer to a given data stored in the experiment
    sptrDataSet dataShortName(const std::string& name);
    //! Get number of data
    int numData() const { return _data.size(); };
    //! Add some data to the experiment
    void addData(const std::string& name, sptrDataSet data);
    //! Add some data to the experiment
    void addData(sptrDataSet data);
    //! Returns true if the experiment has a data
    bool hasData(const std::string& name) const;
    //! Remove a data from the experiment
    void removeData(const std::string& name);

 public: // Peak Collection
    //! Add a peak collection
    void updatePeakCollection(
        const std::string& name, const listtype type, const std::vector<nsx::Peak3D*> peaks);
    //! Returns true if the experiment has a data
    bool hasPeakCollection(const std::string& name) const;
    //! Returns the peak list denoted by the name
    PeakCollection* getPeakCollection(const std::string name);
    //! Remove a data from the experiment
    void removePeakCollection(const std::string& name);
    //! Get a list of loaded list names
    std::vector<std::string> getCollectionNames() const;
    //! Get a list of loaded list names
    std::vector<std::string> getFoundCollectionNames() const;
    //! Get a list of loaded list names
    std::vector<std::string> getPredictedCollectionNames() const;
    //! Get the number of peak lists
    int numPeakCollections() const { return _peak_collections.size(); };
    //! Accept a filtering of the peaks and process it
    void acceptFilter(const std::string name, PeakCollection* collection);
    //! Check for unphysical peaks in all collections
    void checkPeakCollections();

 public: // MergedData
    //! Set the merged peak
    void setMergedPeaks(std::vector<PeakCollection*> peak_collections, bool friedel);
    //! Set merged peaks without the vector (mainly for SWIG)
    void setMergedPeaks(PeakCollection* found, PeakCollection* predicted, bool friedel);
    //! Reset the merged peak
    void resetMergedPeaks();
    //! Reset the merged peak
    MergedData* getMergedPeaks() const { return _merged_peaks.get(); };

 public: // Unit cells
    //! Add some data to the experiment
    void addUnitCell(const std::string& name, UnitCell* unit_cell);
    //! Add a unit cell to the experiment via cell parameters (skip autoindexing step)
    void addUnitCell(const std::string& name, double a, double b, double c,
                     double alpha, double beta, double gamma);
    //! Returns true if the experiment has a data
    bool hasUnitCell(const std::string& name) const;
    //! Get a list of loaded list names
    std::vector<std::string> getUnitCellNames() const;
    //! Returns the unit cell denoted by the name
    UnitCell* getUnitCell(const std::string& name);
    //! Remove a data from the experiment
    void removeUnitCell(const std::string& name);
    //! Remove a data from the experiment
    void swapUnitCells(const std::string& old_cell, const std::string& new_cell);
    //! Get the number of peak lists
    int numUnitCells() const { return _unit_cells.size(); };
    //! Accept an autoindexer solution as the unit cell
    bool acceptUnitCell(PeakCollection* peaks, double length_tol, double angle_tol);
    //! Assign unit cell to a peak collection
    void acceptUnitCell(PeakCollection* peaks);

 public: // Peak finder
    //! Get the address of the peak finder
    nsx::PeakFinder* peakFinder() const { return _peak_finder.get(); };
    //! Transfer current peaks as collection
    void acceptFoundPeaks(const std::string& name);
    //! Get the found peak integrator
    nsx::PeakFilter* peakFilter() { return _peak_filter.get(); };

 public: // Autoindexer
    //! Get the auto indexer
    nsx::AutoIndexer* autoIndexer() const { return _auto_indexer.get(); };
    //! Set the reference cell
    void setReferenceCell(double a, double b, double c, double alpha, double beta, double gamma);
    //! get the accepted Unit Cell
    UnitCell* getAcceptedCell();
    //! get the reference cell
    UnitCell* getReferenceCell();

 public: // Integrator
    nsx::IPeakIntegrator* getIntegrator(const std::string& name) const;
    //! Set the found peak integrator
    void integratePeaks(const std::string& integrator_name, PeakCollection* peak_collection);
    //! Set the found peak integrator
    void integratePredictedPeaks(const std::string& integrator_name, PeakCollection* peak_collection,
                                 ShapeLibrary* shape_library, PredictionParameters& params);
    //! Set the found peak integrator
    void integrateFoundPeaks(const std::string& integrator);

 public: // Save load
    //! Save to file
    bool saveToFile(const std::string& path) const;
    //! Load from file
    bool loadFromFile(const std::string& path);

 public: // Prediction
    //! Build the shape library
    void buildShapeLibrary(PeakCollection* peaks, ShapeLibParameters params);
    //! Get the shape library
    ShapeLibrary* getShapeLibrary() { return &_shape_library; };
    //! Predict peaks
    void predictPeaks(
        const std::string& name, PeakCollection* peaks,
        PredictionParameters params, PeakInterpolation interpol);

 public: // Merging
    //! Get resolution shells for quality metrics
    void computeQuality(
        double d_min, double d_max, int n_shells, PeakCollection* predicted, PeakCollection* found,
        bool friedel);
    //! Return data quality resolution
    std::vector<DataResolution>* getResolution() { return &_data_resolution; };
    //! Return data quality for all merged data
    DataQuality* getQuality() { return &_data_quality; };

 private: // private variables
    //! The name of this experiment
    std::string _name = "No_name";
    //! A pointer to the detector assigned to this experiment
    std::unique_ptr<Diffractometer> _diffractometer;
    //! A map of the data related to the experiment.
    std::map<std::string, sptrDataSet> _data;
    //! A map of the peaklists with their name as index
    std::map<std::string, std::unique_ptr<PeakCollection>> _peak_collections;
    //! A map of the peaklists with their name as index
    std::unique_ptr<MergedData> _merged_peaks;
    //! A map of the unit cells with their name as index
    std::map<std::string, std::unique_ptr<UnitCell>> _unit_cells;
    //! The Peak finder
    std::unique_ptr<nsx::PeakFinder> _peak_finder;
    //! The peak filter
    std::unique_ptr<nsx::PeakFilter> _peak_filter;
    //! The auto indexer
    std::unique_ptr<nsx::AutoIndexer> _auto_indexer;
    //! The found peak integrator
    std::map<std::string, std::unique_ptr<nsx::IPeakIntegrator>> _integrator_map;
    //! Peak shape library for prediction
    ShapeLibrary _shape_library;
    //! Data quality metrics for all merged data
    DataQuality _data_quality;
    //! Data quality metrics as a function of resolution shell
    std::vector<DataResolution> _data_resolution;
};

} // namespace nsx

#endif // CORE_EXPERIMENT_EXPERIMENT_H
