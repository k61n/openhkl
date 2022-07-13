//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/experiment/PeakFinder.h
//! @brief     Defines class PeakFinder
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef NSX_CORE_EXPERIMENT_PEAKFINDER_H
#define NSX_CORE_EXPERIMENT_PEAKFINDER_H

#include "core/convolve/Convolver.h"
#include "core/data/DataSet.h"
#include "core/shape/PeakCollection.h"

namespace ohkl {

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
    int frames_begin = -1; //!< First frame in range for peak finding
    int frames_end = -1; //!< Last frame in range for peak findinng
    double threshold = 80.0; //!< Blobs containing fewer counts than this are discarded
    std::string convolver = "annular";

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
    void find(const DataList numors);

    //! Return a vector of peaks found by PeakFinder
    std::vector<Peak3D*> currentPeaks();

    //! Return a pointer to the PeakCollection owned by PeakFinder
    PeakCollection* getPeakCollection();
    //! Generate a PeakCollection from a vector of found peaks
    void setPeakCollection(
        const std::string name, const ohkl::PeakCollectionType type,
        std::vector<std::shared_ptr<ohkl::Peak3D>> peak_list);

    //! Return the DataList (vector of pointers to DataSets)
    DataList currentData() { return _current_data; };

    //! Set the progress handler
    void setHandler(const sptrProgressHandler& handler);

    //! Get the parameters for peak finding
    PeakFinderParameters* parameters();

    //! Get the number of peaks found
    unsigned int numberFound();

    //! Return integrated state
    bool isIntegrated() { return _integrated; };

    //! set integration state
    void setIntegrated(bool b) { _integrated = b; };

#ifndef SWIG
    //! Set the convolver flavour for peak/background convolution
    void setConvolver(std::unique_ptr<Convolver> convolver);
#endif

    //! Set the convolver flavour for peak/background convolution
    void setConvolver(const Convolver& convolver);
    //! Get the convolver
    ohkl::Convolver* convolver() const
    {
        return _convolver.get();
    }

 private:
    //! Remove blobs that do not meet the above criteria
    void eliminateBlobs(std::map<int, Blob3D>& blobs) const;

    //! Detector spot detection algorithm
    void findPrimaryBlobs(
        const DataSet& data, std::map<int, Blob3D>& blobs, EquivalenceList& equivalences,
        size_t begin, size_t end, int n_numor);

    //! Detect collisions between blobs
    void findCollisions(
        const DataSet& data, std::map<int, Blob3D>& blobs, EquivalenceList& equivalences) const;

    //! Merge blobs that collide
    void mergeCollidingBlobs(const DataSet& data, std::map<int, Blob3D>& blobs) const;

    //! Merge equivalent blobs
    void mergeEquivalentBlobs(std::map<int, Blob3D>& blobs, EquivalenceList& equivalences) const;

 private:
    sptrProgressHandler _handler;

    //! The convolver for peak/background convolution
    std::unique_ptr<Convolver> _convolver;
    //! The parameters for peak finding
    std::unique_ptr<PeakFinderParameters> _params;
    //! Current label
    int _current_label;
    //! Vector of found peaks
    ohkl::PeakList _current_peaks;
    //! Vector of DataSets
    ohkl::DataList _current_data;
    //! PeakCollection attached to PeakFinder
    ohkl::PeakCollection _peak_collection;
    //! Number of found peaks
    unsigned int _peaks_found;
    //! stores value of integration state
    bool _integrated;
};

/*! @}*/
} // namespace ohkl

#endif // NSX_CORE_EXPERIMENT_PEAKFINDER_H
