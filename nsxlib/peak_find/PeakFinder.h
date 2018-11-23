#pragma once

#include <memory>

#include "Blob3D.h"
#include "Convolver.h"
#include "CrystalTypes.h"
#include "DataTypes.h"
#include "GeometryTypes.h"
#include "ITask.h"
#include "UtilsTypes.h"

namespace nsx {

//! \brief Class to handle peak search in datasets
class PeakFinder : public ITask {

public:

    PeakFinder(const DataList &datasets);

    bool doTask() final;

    void setPeakScale(double scale);

    void setMinSize(int minComp);
    int minSize() const;

    void setMaxSize(int maxComp);
    int maxSize() const;

    void setMaxFrames(int maxComp);
    int maxFrames() const;

    const PeakList& peaks() const;

#ifndef SWIG
    void setConvolver(std::unique_ptr<Convolver> convolver);
#endif

    void setConvolver(const Convolver& convolver);

    void setThreshold(double value);

private:

    void eliminateBlobs(std::map<int, Blob3D>& blobs) const;

    void findPrimaryBlobs(const DataSet &data, std::map<int,Blob3D>& blobs,EquivalenceList& equivalences, size_t begin, size_t end);

    void findCollisions(const DataSet &data, std::map<int,Blob3D>& blobs, EquivalenceList& equivalences) const;

    void mergeCollidingBlobs(const DataSet &data, std::map<int,Blob3D>& blobs) const;

    void mergeEquivalentBlobs(std::map<int,Blob3D>& blobs, EquivalenceList& equivalences) const;

private:

    std::unique_ptr<Convolver> _convolver;

    DataList _datasets;

    double _threshold;

    double _peakScale;

    int _current_label;

    int _minSize;

    int _maxSize;

    int _maxFrames;

    PeakList _peaks;
};

} // end namespace nsx
