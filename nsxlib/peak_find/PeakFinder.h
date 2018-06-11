#pragma once

#include "Blob3D.h"
#include "Convolver.h"
#include "CrystalTypes.h"
#include "DataTypes.h"
#include "GeometryTypes.h"
#include "PeakFindTypes.h"
#include "UtilsTypes.h"

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

    sptrConvolver convolver() const;
    void setConvolver(const std::string& convolver_type, const std::map<std::string,double>& parameters);

    void setThreshold(double value);

private:

    void eliminateBlobs(std::map<int, Blob3D>& blobs) const;

    void findPrimaryBlobs(sptrDataSet data, std::map<int,Blob3D>& blobs,EquivalenceList& equivalences, size_t begin, size_t end);

    void findCollisions(sptrDataSet data, std::map<int,Blob3D>& blobs, EquivalenceList& equivalences) const;

    void mergeCollidingBlobs(sptrDataSet data, std::map<int,Blob3D>& blobs) const;

    void mergeEquivalentBlobs(std::map<int,Blob3D>& blobs, EquivalenceList& equivalences) const;

private:
    sptrProgressHandler _handler;
    sptrConvolver _convolver;

    double _threshold;

    double _peakScale;

    int _current_label;

    int _minSize;
    int _maxSize;

    int _maxFrames;
};

} // end namespace nsx
