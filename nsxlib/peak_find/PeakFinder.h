#pragma once

#include "Blob3D.h"
#include "Convolver.h"
#include "CrystalTypes.h"
#include "DataTypes.h"
#include "GeometryTypes.h"
#include "ImagingTypes.h"
#include "UtilsTypes.h"

namespace nsx {

//! Class to handle peak search in datasets
class PeakFinder {

public:

    PeakFinder();

    PeakList find(DataList numors);

    void setHandler(const sptrProgressHandler& handler);

    void setThresholdValue(double threshold);
    double getThresholdValue();

    void setThresholdType(int type);
    int getThresholdType();

    void setIntegrationConfidence(double confidence);
    double integrationConfidence() const;

    void setSearchConfidence(double confidence);
    double searchConfidence() const;

    void setMinComponents(int minComp);
    int getMinComponents();

    void setMaxComponents(int maxComp);
    int getMaxComponents();

    sptrConvolutionKernel kernel() const;
    void setKernel(const std::string& kernel_type, const std::map<std::string,double>& parameters);

private:

    void eliminateBlobs(std::map<int, Blob3D>& blobs) const;

    void findPrimaryBlobs(sptrDataSet data, std::map<int,Blob3D>& blobs,EquivalenceList& equivalences, size_t begin, size_t end);

    void findCollisions(sptrDataSet data, std::map<int,Blob3D>& blobs, EquivalenceList& equivalences) const;

    void mergeCollidingBlobs(sptrDataSet data, std::map<int,Blob3D>& blobs) const;

    void mergeEquivalentBlobs(std::map<int,Blob3D>& blobs, EquivalenceList& equivalences) const;

private:
    sptrProgressHandler _handler;

    Convolver _convolver;

    sptrConvolutionKernel _kernel;

    double _thresholdValue;
    int _thresholdType;

    double _searchConfidence;
    double _integrationConfidence;
    double _median;

    int _current_label;

    int _minComp;
    int _maxComp;
};

} // end namespace nsx
