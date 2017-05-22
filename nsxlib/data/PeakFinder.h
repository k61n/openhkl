
#ifndef NSXLIB_PEAKFINDER_H
#define NSXLIB_PEAKFINDER_H

#include "../data/DataTypes.h"
#include "../imaging/ImagingTypes.h"
#include "../utils/UtilsTypes.h"

namespace nsx {

class PeakFinder {

public:

    PeakFinder();

    bool find(DataList numors);

    void setHandler(const sptrProgressHandler& handler);

    void setThresholdValue(double threshold);
    double getThresholdValue();

    void setThresholdType(int type);
    int getThresholdType();

    void setConfidence(double confidence);
    double confidence() const;

    void setMinComponents(int minComp);
    int getMinComponents();

    void setMaxComponents(int maxComp);
    int getMaxComponents();

    void setConvolver(sptrConvolver convolver);
    sptrConvolver getConvolver();

    sptrConvolutionKernel getKernel();

    void setKernel(sptrConvolutionKernel kernel);

private:
    sptrProgressHandler _handler;
    sptrConvolver _convolver;
    sptrConvolutionKernel _kernel;

    double _thresholdValue;
    int _thresholdType;

    double _confidence;
    double _median;

    int _minComp;
    int _maxComp;
};

} // end namespace nsx

#endif // NSXLIB_PEAKFINDER_H
