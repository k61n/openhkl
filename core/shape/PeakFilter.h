//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/shape/PeakFilter.h
//! @brief     Defines class PeakFilter
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_CORE_SHAPE_PEAKFILTER_H
#define OHKL_CORE_SHAPE_PEAKFILTER_H

#include "core/peak/Peak3D.h"
#include "core/raw/DataKeys.h"

namespace ohkl {

class PeakCollection;
class InstrumentState;

/*! \addtogroup python_api
 *  @{*/

//! Enable/disable the various types of filters
struct PeakFilterFlags {
    bool enabled; //!< filter by enabled
    bool masked; //!< filter by mask
    bool indexed; //!< filter by indexed peaks
    bool index_tol; //!
    bool strength; //!< filter by strength (intensity/sigma)
    bool d_range; //!< filter by detector range
    bool extinct; //!< filter by extinction (allowed by unit cell)
    bool sparse; //!
    bool significance; //!
    bool overlapping; //!
    bool complementary; //!
    bool frames; //!< catch peaks in a specifed frame range
    bool rejection_flag; //!< catch peaks with a specific rejection flag
    bool intensity; //!< filter by intensity
    bool sigma; //!< filter by sigma
    bool gradient; //!< filter by background gradient
    bool gradient_sigma; //!< filter by background gradient sigma
};

//! Parameters for the different filter types
struct PeakFilterParameters {
    double d_min = 1.5; //!< minimum d (Bragg's law)
    double d_max = 50.0; //!< maximum d (Bragg's law)
    double strength_min = 1.0; //!< minimum strength (I/sigma)
    double strength_max = 1.0e7; //!< maximum strength (I/sigma)
    double intensity_min = 0.0; //!< minimum intensity
    double intensity_max = 1.0e7; //!< maximum intensity
    double sigma_min = 0.0; //!< minimum sigma
    double sigma_max = 1000.0; //!< maximum sigma
    double gradient_min = 0.0;
    double gradient_max = 1000.0;
    double gradient_sigma_min = 0.0;
    double gradient_sigma_max = 1000.0;
    std::string unit_cell = ""; //!< unit cell name
    double unit_cell_tolerance = 0.2; //!< indexing tolerance
    double significance = 0.99; //!< signficance
    double sparse = 100; //!< number of peaks in dataset to be kept
    double first_frame = 0.0; //!< start of frame range
    double last_frame = 9.0; //!< end of frame range
    double peak_end = 3.0; //!< scale for peak intensity ellipsoid (sigmas)
    double bkg_end = 6.0; //!< scale for background ellipsoid (sigmas)
    RejectionFlag rejection_flag = RejectionFlag::NotRejected; //!< rejection flag to keep
    bool sum_intensities = true;
};

/*! \brief Remove peaks that meet specific criteria from a collection
 *
 *  Peaks excluded by the filter will be labelled "caught" by the filter.
 */

class PeakFilter {
 public:
    //! Constructor
    PeakFilter();

 public:
    //! set filter parameters to default
    void resetFilterFlags();

    //! Filter peaks that are complementary to the given peaks
    void filterComplementary(PeakCollection* peak_collection) const;

    //! Filter only enabled peaks
    void filterEnabled(PeakCollection* peak_collection) const;

    //! Filter only masked peaks
    void filterMasked(PeakCollection* peak_collection) const;

    //! Filter only peaks indexed by their unit cell and its indexing tolerance
    void filterIndexed(PeakCollection* peak_collection) const;

    //! Filter only peaks indexed by a given unit cell with a given indexing
    //! tolerance
    void filterIndexTolerance(PeakCollection* peak_collection) const;

    //! Keeps only the peaks whose unit cell is the given unit cell
    void filterUnitCell(PeakCollection* peak_collection) const;

    //! Remove peaks whitout unit cell
    void filterHasUnitCell(PeakCollection* peak_collection) const;

    //! Filter peaks with I/sigma above threshold
    void filterStrength(PeakCollection* peak_collection) const;

    //! Remove peaks which are not in a d-range
    void filterDRange(PeakCollection* peak_collection) const;

    //! Filter merged peaks which satisfies a chi2 test
    void filterSignificance(PeakCollection* peak_collection) const;

    //! Remove overlapping peaks
    void filterOverlapping(PeakCollection* peak_collection) const;

    //! Remove space-group extincted peaks
    void filterExtinct(PeakCollection* peak_collection) const;

    //! Remove peaks which belongs to datasets containing too few peaks
    void filterSparseDataSet(PeakCollection* peak_collection) const;

    //! Remove peaks outside the specified frame range
    void filterFrameRange(PeakCollection* peak_collection) const;

    //! Remove peaks *without* the specified rejection flag
    void filterRejectionFlag(PeakCollection* peak_collection) const;

    //! Remove peaks outside the given intensity range
    void filterIntensity(PeakCollection* peak_collection) const;

    //! Remove peaks outside the given sigma range
    void filterSigma(PeakCollection* peak_collection) const;

    //! Remove peaks outside given background gradient range
    void filterGradient(PeakCollection* peak_collection) const;

    //! Remove peaks outside given background gradient sigma range
    void filterGradientSigma(PeakCollection* peak_collection) const;

    //! Filter only enabled on a peak vector
    std::vector<Peak3D*> filterEnabled(const std::vector<Peak3D*>& peaks, bool flag) const;

    //! Filter only enabled on a peak vector
    std::vector<Peak3D*> filterIndexed(
        const std::vector<Peak3D*>& peaks, const UnitCell* cell = nullptr,
        const InstrumentState* state = nullptr) const;

    //! Remove peaks from outside a frame range for a vector
    std::vector<Peak3D*> filterFrameRange(
        const std::vector<Peak3D*>& peaks, int first_frame, int last_frame) const;

    //! Filter d-range on peak vector (intended for a single frame, hence the state)
    std::vector<Peak3D*> filterDRange(
        const std::vector<Peak3D*>& peaks, double d_min, double d_max,
        const InstrumentState* state = nullptr) const;

    //! Filter strength on peak vector
    std::vector<Peak3D*> filterStrength(
        const std::vector<Peak3D*>& peaks, double str_min, double str_max);

    //! Run the filtering
    void filter(PeakCollection* peak_collection) const;

    //! Reset filtering
    void resetFiltering(PeakCollection* peak_collection) const;

    //! Get a pointer to the filter parameters
    PeakFilterParameters* parameters();
    //! Get pointer to filter flags
    PeakFilterFlags* flags();

 private:
    Intensity peakIntensity(Peak3D* peak) const;
    //! booleans for filtering
    std::unique_ptr<PeakFilterFlags> _filter_flags;
    std::unique_ptr<PeakFilterParameters> _filter_params;
};

/*! @}*/
} // namespace ohkl

#endif // OHKL_CORE_SHAPE_PEAKFILTER_H
