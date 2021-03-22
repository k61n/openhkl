//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
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

namespace nsx {

using EquivalencePair = std::pair<int, int>;
using EquivalenceList = std::vector<EquivalencePair>;

class Blob3D;
class ProgressHandler;

using sptrProgressHandler = std::shared_ptr<ProgressHandler>;

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
        const std::string name, const nsx::listtype type,
        std::vector<std::shared_ptr<nsx::Peak3D>> peak_list);

    //! Return the DataList (vector of pointers to DataSets)
    DataList currentData() { return _current_data; };

    //! Set the progress handler
    void setHandler(const sptrProgressHandler& handler);

    //! Return the peak scale
    double peakEnd() const { return _peakEnd; }
    //! Set the peak scale
    void setPeakEnd(double scale) { _peakEnd = scale; }

    //! Return the minimum blob size. Blob is discarded if it contains fewer pixels.
    int minSize() const;
    //! Set the minimum blob size
    void setMinSize(int minComp);

    //! Return the maximum blob size. Blob is discarded if it contains more pixels.
    int maxSize() const;
    //! Set the maximum blob size
    void setMaxSize(int maxComp);

    //! Return the maximum number of frames for a blob. Blob is discarded if it is
    //! spread over more frames.
    int maxFrames() const;
    //! Set the maximum number of frames for a blob
    void setMaxFrames(int maxComp);

    //! Return the frame at which peak finding begins
    int framesBegin() const;
    //! Set the frame at which peak finding begins
    void setFramesBegin(int framesBegin);

    //! Return the frame at which peak finding ends
    int framesEnd() const;
    //! Set the frame at which peak finding ends
    void setFramesEnd(int framesEnd);

#ifndef SWIG
    //! Set the convolver flavour for peak/background convolution
    void setConvolver(std::unique_ptr<Convolver> convolver);
#endif

    //! Set the convolver flavour for peak/background convolution
    void setConvolver(const Convolver& convolver);
    //! Get the convolver
    nsx::Convolver* convolver() const { return _convolver.get(); }

    //! Get the threshold. Blobs containing fewer counts than the threshold are
    //! discarded
    int threshold() const { return _threshold; };
    //! Set the threshold.
    void setThreshold(double value);

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
    //! The threshold (counts) for discarding a blob
    double _threshold;
    //! The peak scaling factor
    double _peakEnd;
    //! Label defining type of pixel (peak, background etc.)
    int _current_label;
    //! Minimum number of pixels for a blob
    int _minSize;
    //! Maximum number of pixels for a blob
    int _maxSize;
    //! Maximum number of frames a blob can span
    int _maxFrames;
    //! First frame in range for peak finding
    int _framesBegin;
    //! Last frame in range for peak finding
    int _framesEnd;
    //! Vector of found peaks
    nsx::PeakList _current_peaks;
    //! Vector of DataSets
    nsx::DataList _current_data;
    //! PeakCollection attached to PeakFinder
    nsx::PeakCollection _peak_collection;
};

} // namespace nsx

#endif // NSX_CORE_EXPERIMENT_PEAKFINDER_H
