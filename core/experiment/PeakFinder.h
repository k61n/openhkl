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

//! Class to handle peak search in datasets

class PeakFinder {
 public:
    PeakFinder();

    void find(const DataList numors);

    std::vector<Peak3D*> currentPeaks();

    PeakCollection* getPeakCollection();
    void setPeakCollection(
        const std::string name, const nsx::listtype type,
        std::vector<std::shared_ptr<nsx::Peak3D>> peak_list);

    DataList currentData() { return _current_data; };

    void setHandler(const sptrProgressHandler& handler);

    void setPeakEnd(double scale) { _peakEnd = scale; }
    double peakEnd() const { return _peakEnd; }

    void setMinSize(int minComp);
    int minSize() const;

    void setMaxSize(int maxComp);
    int maxSize() const;

    void setMaxFrames(int maxComp);
    int maxFrames() const;

    void setFramesBegin(int framesBegin);
    int framesBegin();

    void setFramesEnd(int framesEnd);
    int framesEnd();

#ifndef SWIG
    void setConvolver(std::unique_ptr<Convolver> convolver);
#endif

    void setConvolver(const Convolver& convolver);
    nsx::Convolver* convolver() const { return _convolver.get(); }

    void setThreshold(double value);
    int threshold() const { return _threshold; };

 private:
    void eliminateBlobs(std::map<int, Blob3D>& blobs) const;

    void findPrimaryBlobs(
        const DataSet& data, std::map<int, Blob3D>& blobs, EquivalenceList& equivalences,
        size_t begin, size_t end, int n_numor);

    void findCollisions(
        const DataSet& data, std::map<int, Blob3D>& blobs, EquivalenceList& equivalences) const;

    void mergeCollidingBlobs(const DataSet& data, std::map<int, Blob3D>& blobs) const;

    void mergeEquivalentBlobs(std::map<int, Blob3D>& blobs, EquivalenceList& equivalences) const;

 private:
    sptrProgressHandler _handler;

    std::unique_ptr<Convolver> _convolver;

    double _threshold;

    double _peakEnd;

    int _current_label;

    int _minSize;

    int _maxSize;

    int _maxFrames;

    int _framesBegin;

    int _framesEnd;

    nsx::PeakList _current_peaks;

    nsx::DataList _current_data;

    nsx::PeakCollection _peak_collection;
};

} // namespace nsx

#endif // NSX_CORE_EXPERIMENT_PEAKFINDER_H
