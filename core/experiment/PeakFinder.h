//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/experiment/PeakFinder.h
//! @brief     Defines class PeakFinder
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_CORE_EXPERIMENT_PEAKFINDER_H
#define OHKL_CORE_EXPERIMENT_PEAKFINDER_H

#include "core/data/DataTypes.h"
#include "core/shape/PeakCollection.h"

namespace ohkl {

class ImageFilter;

/*! \addtogroup python_api
 *  @{*/

using EquivalencePair = std::pair<int, int>;
using EquivalenceList = std::vector<EquivalencePair>;

class Blob3D;
class ProgressHandler;

using sptrProgressHandler = std::shared_ptr<ProgressHandler>;

//! Parameters for finding peaks
struct PeakFinderParameters {
    int minimum_size = 30; //!< Minimum number of pixels in a blob
    int maximum_size = 10000; //!< Maximum number of pixels in a blob
    double peak_end = 1.0; //!< Peak scaling factor (sigmas)
    int maximum_frames = 10; //!< Maximum number of frames a blob can span
    int first_frame = -1; //!< First frame in range for peak finding
    int last_frame = -1; //!< Last frame in range for peak findinng
    double r1 = 5.0; //!< Upper bound for positive region of filter kernel
    double r2 = 10.0; //!< Lower bound for negative region of filter kernel
    double r3 = 15.0; //!< Upper bound for negative region of filter kernel
    double threshold = 1.0; //!< Blobs containing fewer counts than this are discarded
    std::string filter = "Enhanced annular";

    void log(const Level& level) const;
};

/*! \brief Perform image recognition on detector images to find peaks
 *
 *  Given a "DataSet" (set of detector images from one experimental run),
 *  PeakFinder will locate all detector spots per-frame, and merge sets of
 *  contiguous partial reflections into a blog in 3D space (a "Blob3D"). A blob
 *  will become a Bragg reflexis for which an intensity and sigma can be
 *  calculated.
 */

class PeakFinder {
 public:
    //! Default constructor
    PeakFinder();

    //! Iterate through a vector of DataSets, finding the peaks for each
    void find(const sptrDataSet data);

    //! Return a vector of peaks found by PeakFinder
    std::vector<Peak3D*> currentPeaks();

    //! Return a pointer to the PeakCollection owned by PeakFinder
    PeakCollection* getPeakCollection();
    //! Generate a PeakCollection from a vector of found peaks
    void setPeakCollection(
        const std::string name, const ohkl::PeakCollectionType type,
        std::vector<std::shared_ptr<ohkl::Peak3D>> peak_list, sptrDataSet data);

    //! Return the DataList (vector of pointers to DataSets)
    sptrDataSet currentData() { return _current_data; };

    //! Set the progress handler
    void setHandler(const sptrProgressHandler& handler);

    //! Get the parameters for peak finding
    PeakFinderParameters* parameters();

    //! Get the number of peaks found
    unsigned int numberFound();

    //! Have we found peaks?
    bool foundPeaks() const { return !_current_peaks.empty(); };

    //! Return integrated state
    bool isIntegrated() { return _integrated; };

    //! Return background gradient status
    bool hasBkgGradient() { return _gradient; };

    //! set integration state
    void setIntegrated(bool b) { _integrated = b; };

    //! set background gradient status
    void setBkgGradient(bool b) { _gradient = b; };

    //! Set image filter parameters
    void setFilterParameters(const std::map<std::string, double>& params);

    //! Get the image filter parameters
    std::map<std::string, double> filterParameters();

 private:
    //! Remove blobs that do not meet the above criteria
    void eliminateBlobs(std::map<int, Blob3D>& blobs) const;

    //! Detector spot detection algorithm
    void findPrimaryBlobs(
        const DataSet& data, ImageFilter* filter, std::map<int, Blob3D>& blobs,
        EquivalenceList& equivalences, size_t begin, size_t end);

    //! Detect collisions between blobs
    void findCollisions(
        const DataSet& data, std::map<int, Blob3D>& blobs, EquivalenceList& equivalences) const;

    //! Merge blobs that collide
    void mergeCollidingBlobs(const DataSet& data, std::map<int, Blob3D>& blobs) const;

    //! Merge equivalent blobs
    void mergeEquivalentBlobs(std::map<int, Blob3D>& blobs, EquivalenceList& equivalences) const;

    sptrProgressHandler _handler;

    //! The parameters for peak finding
    std::unique_ptr<PeakFinderParameters> _params;
    //! Current label
    int _current_label;
    //! Vector of found peaks
    PeakList _current_peaks;
    //! Current DataSet
    sptrDataSet _current_data;
    //! PeakCollection attached to PeakFinder
    PeakCollection _peak_collection;
    //! Number of found peaks
    unsigned int _peaks_found;
    //! stores value of integration state
    bool _integrated;
    //! whether peaks have background gradient after integration
    bool _gradient;

    static const double _eps;
};

/*! @}*/
} // namespace ohkl

#endif // OHKL_CORE_EXPERIMENT_PEAKFINDER_H
