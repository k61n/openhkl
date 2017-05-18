
#ifndef NSXTOOL_PEAKFINDER_H_
#define NSXTOOL_PEAKFINDER_H_

#include <memory>

#include "../imaging/Convolver.h"
#include "../imaging/ConvolutionKernel.h"

namespace nsx {

class DataSet;
class ProgressHandler;

class PeakFinder {

public:

    PeakFinder();

    bool find(std::vector<std::shared_ptr<DataSet>> numors);

    void setHandler(const std::shared_ptr<ProgressHandler>& handler);

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

    void setConvolver(std::shared_ptr<Convolver> convolver);
    std::shared_ptr<Convolver> getConvolver();

    std::shared_ptr<ConvolutionKernel> getKernel();

    void setKernel(std::shared_ptr<ConvolutionKernel> kernel);



private:
    std::shared_ptr<ProgressHandler> _handler;
    std::shared_ptr<Convolver> _convolver;
    std::shared_ptr<ConvolutionKernel> _kernel;

    double _thresholdValue;
    int _thresholdType;

    double _confidence;
    double _median;

    int _minComp;
    int _maxComp;
};

} // end namespace nsx

#endif // PEAKFINDER_H
