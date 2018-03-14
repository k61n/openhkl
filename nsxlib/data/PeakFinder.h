#pragma once

#include "CrystalTypes.h"
#include "DataTypes.h"
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

    void setIntegrationScale(double scale);
    double integrationScale() const;

    void setBackgroundScale(double scale);
    double backgroundScale() const;


    void setSearchScale(double scale);
    double searchScale() const;

    void setMinComponents(int minComp);
    int getMinComponents();

    void setMaxComponents(int maxComp);
    int getMaxComponents();

    void setConvolver(sptrConvolver convolver);
    sptrConvolver convolver();

    sptrConvolutionKernel kernel();

    void setKernel(sptrConvolutionKernel kernel);

private:
    sptrProgressHandler _handler;
    sptrConvolver _convolver;
    sptrConvolutionKernel _kernel;

    double _thresholdValue;
    int _thresholdType;

    double _searchScale;
    double _integrationScale;
    double _backgroundScale;
    double _median;

    int _minComp;
    int _maxComp;
};

} // end namespace nsx
