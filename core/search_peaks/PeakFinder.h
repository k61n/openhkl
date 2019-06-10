//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/search_peaks/PeakFinder.h
//! @brief     Defines ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef CORE_SEARCH_PEAKS_PEAKFINDER_H
#define CORE_SEARCH_PEAKS_PEAKFINDER_H

#include "core/search_peaks/Blob3D.h"
#include "core/search_peaks/Convolver.h"
#include "core/utils/UtilsTypes.h"

namespace nsx {

//! Class to handle peak search in datasets
class PeakFinder {

public:
    PeakFinder();

    PeakList find(DataList numors);

    void setHandler(const sptrProgressHandler& handler);

    void setPeakScale(double scale) { _peakScale = scale; }

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

    void setThreshold(double value);

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
};

} // namespace nsx

#endif // CORE_SEARCH_PEAKS_PEAKFINDER_H
