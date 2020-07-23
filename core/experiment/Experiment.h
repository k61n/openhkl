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
    double Rmerge; //!< R-factor
    double Rmeas; //!< multiplicity-weighted R-factor
    double Rpim; //!< relative (precision-indicating) R-factor
    double CChalf; //!< CC_{1/2} correlation coefficient
};

struct DataResolution {
    double dmin; //!< Lower limit of d for resolution shell
    double dmax; //!< Upper limit of d for resolution shell
    DataQuality currentQuality;
    DataQuality expectedQuality;
};

class Experiment {
 public:
    Experiment() = delete;
    Experiment(const std::string& name, const std::string& diffractometerName);
    ~Experiment() = default;

    Experiment(const Experiment& other) = delete;

    // General
    const std::string& name() const;
    void setName(const std::string& name);

    Diffractometer* diffractometer();
    const Diffractometer* diffractometer() const;
    void setDiffractometer(const std::string& diffractometerName);

    // Data sets
    const std::map<std::string, sptrDataSet>& getDataMap() const;
    sptrDataSet getData(const std::string& name);
    DataList getAllData();
    sptrDataSet dataShortName(const std::string& name);
    int numData() const { return _data_map.size(); };
    void addData(const std::string& name, sptrDataSet data);
    void addData(sptrDataSet data);
    bool hasData(const std::string& name) const;
    void removeData(const std::string& name);

    // Peak Collection
    void updatePeakCollection(
        const std::string& name, const listtype type, const std::vector<nsx::Peak3D*> peaks);
    bool hasPeakCollection(const std::string& name) const;
    PeakCollection* getPeakCollection(const std::string name);
    void removePeakCollection(const std::string& name);
    std::vector<std::string> getCollectionNames() const;
    std::vector<std::string> getFoundCollectionNames() const;
    std::vector<std::string> getPredictedCollectionNames() const;
    int numPeakCollections() const { return _peak_collections.size(); };
    void acceptFilter(const std::string name, PeakCollection* collection);
    void checkPeakCollections();

    // MergedData
    void setMergedPeaks(std::vector<PeakCollection*> peak_collections, bool friedel);
    // ditto without the vector (mainly for SWIG):
    void setMergedPeaks(PeakCollection* found, PeakCollection* predicted, bool friedel);
    void resetMergedPeaks();
    MergedData* getMergedPeaks() const { return _merged_peaks.get(); };

    // Unit cells
    void addUnitCell(const std::string& name, UnitCell* unit_cell);
    //! Add a unit cell to the experiment via cell parameters (skip autoindexing step)
    void addUnitCell(
        const std::string& name, double a, double b, double c, double alpha, double beta,
        double gamma);
    bool hasUnitCell(const std::string& name) const;
    std::vector<std::string> getUnitCellNames() const;
    const UnitCell* getUnitCell(const std::string& name) const;
    UnitCell* getUnitCell(const std::string& name);
    void removeUnitCell(const std::string& name);
    void swapUnitCells(const std::string& old_cell, const std::string& new_cell);
    int numUnitCells() const { return _unit_cells.size(); };
    bool acceptUnitCell(PeakCollection* peaks, double length_tol, double angle_tol);
    void acceptUnitCell(PeakCollection* peaks);
    std::vector<std::string> getCompatibleSpaceGroups() const;

    // Peak finder
    nsx::PeakFinder* peakFinder() { return _peak_finder.get(); };
    void acceptFoundPeaks(const std::string& name);
    nsx::PeakFilter* peakFilter() { return _peak_filter.get(); };

    // Autoindexer
    nsx::AutoIndexer* autoIndexer() const { return _auto_indexer.get(); };
    void setReferenceCell(double a, double b, double c, double alpha, double beta, double gamma);
    const UnitCell* getAcceptedCell() const;
    const UnitCell* getReferenceCell() const;

    // Integrator
    nsx::IPeakIntegrator* getIntegrator(const std::string& name) const;
    void integratePeaks(const std::string& integrator_name, PeakCollection* peak_collection);
    void integratePredictedPeaks(
        const std::string& integrator_name, PeakCollection* peak_collection,
        ShapeLibrary* shape_library, PredictionParameters& params);
    void integrateFoundPeaks(const std::string& integrator);

    // Save load
    void saveToFile(const std::string& path) const;
    void loadFromFile(const std::string& path);

    // Prediction
    void buildShapeLibrary(PeakCollection* peaks, ShapeLibParameters params);
    ShapeLibrary* getShapeLibrary() { return &_shape_library; };
    void predictPeaks(
        const std::string& name, PeakCollection* peaks, PredictionParameters params,
        PeakInterpolation interpol);
    void refine(const PeakCollection* peaks, UnitCell* cell, DataSet* data, int n_batches);

    // Merging
    //! Get resolution shells for quality metrics
    void computeQuality(
        double d_min, double d_max, int n_shells, PeakCollection* predicted, PeakCollection* found,
        bool friedel);
    //! Return data quality resolution
    std::vector<DataResolution>* getResolution() { return &_data_resolution; };
    // Return data quality structs for all merged data:
    const DataQuality& getQualityCurrent() { return _data_quality_current; };
    const DataQuality& getQualityExpected() { return _data_quality_expected; };

 private: // private variables
    std::string _name; //!< The name of this experiment
    std::unique_ptr<Diffractometer> _diffractometer;
    std::map<std::string, sptrDataSet> _data_map;

    //! A map of the peaklists with their name as index
    std::map<std::string, std::unique_ptr<PeakCollection>> _peak_collections;

    //! A map of the peaklists with their name as index
    std::unique_ptr<MergedData> _merged_peaks;

    //! A map of the unit cells with their name as index
    std::map<std::string, std::unique_ptr<UnitCell>> _unit_cells;

    std::unique_ptr<nsx::PeakFinder> _peak_finder;
    std::unique_ptr<nsx::PeakFilter> _peak_filter;
    std::unique_ptr<nsx::AutoIndexer> _auto_indexer;

    //! The found peak integrator
    std::map<std::string, std::unique_ptr<nsx::IPeakIntegrator>> _integrator_map;

    //! Peak shape library for prediction
    ShapeLibrary _shape_library;

    DataQuality _data_quality_current;
    DataQuality _data_quality_expected;

    std::vector<DataResolution> _data_resolution; //!< Data quality per resolution shell
};

} // namespace nsx

#endif // NSX_CORE_EXPERIMENT_EXPERIMENT_H
