//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/analyse/PeakFinder.h
//! @brief     Defines class PeakFinder
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef CORE_ANALYSE_PEAKFINDER_H
#define CORE_ANALYSE_PEAKFINDER_H

#include "base/utils/ProgressHandler.h"
#include "core/analyse/Blob3D.h"
#include "core/convolve/Convolver.h"
#include "core/experiment/DataSet.h"

namespace nsx {

using EquivalencePair = std::pair<int, int>;
using EquivalenceList = std::vector<EquivalencePair>;

//! Class to handle peak search in datasets

class PeakFinder {
 public:
    PeakFinder();

    void find(DataList numors);

    PeakList* currentPeaks() {return &_current_peaks;};

    DataList  currentData() {return _current_data;};

    void setHandler(const sptrProgressHandler& handler);

    void setPeakScale(double scale) { _peakScale = scale; }
    double peakScale() const { return _peakScale; }

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
    nsx::Convolver* convolver() const {return _convolver.get();}

    void setThreshold(double value);
    int threshold() const {return _threshold;};

 private:
    void eliminateBlobs(std::map<int, Blob3D>& blobs) const;

    void findPrimaryBlobs(
        sptrDataSet data, std::map<int, Blob3D>& blobs, EquivalenceList& equivalences, size_t begin,
        size_t end);

    void findCollisions(
        sptrDataSet data, std::map<int, Blob3D>& blobs, EquivalenceList& equivalences) const;

    void mergeCollidingBlobs(sptrDataSet data, std::map<int, Blob3D>& blobs) const;

    void mergeEquivalentBlobs(std::map<int, Blob3D>& blobs, EquivalenceList& equivalences) const;

 private:
    sptrProgressHandler _handler;

    std::unique_ptr<Convolver> _convolver;

    double _threshold;

    double _peakScale;

    int _current_label;

    int _minSize;

    int _maxSize;

    int _maxFrames;

    int _framesBegin;

    int _framesEnd;

    nsx::PeakList _current_peaks;

    nsx::DataList _current_data;

    
};

using sptrPeakFinder = std::shared_ptr<PeakFinder>;

} // namespace nsx

#endif // CORE_ANALYSE_PEAKFINDER_H
