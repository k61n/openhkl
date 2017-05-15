
#ifndef NSXTOOL_PEAKFINDER_H_
#define NSXTOOL_PEAKFINDER_H_

#include <memory>

#include "../imaging/Convolver.h"
#include "../imaging/ConvolutionKernel.h"


namespace nsx {

class Convolver;

class ProgressHandler;

class DataSet;

class PeakFinder {
public:
    PeakFinder();
    bool find(std::vector<std::shared_ptr<DataSet>> numors);

    void setHandler(const std::shared_ptr<nsx::Utils::ProgressHandler>& handler);

    void setThresholdValue(double threshold);
    double getThresholdValue();

    void setThresholdType(int type);
    int getThresholdType();

    void setConfidence(double confidence);
    double getConfidence();

    void setMinComponents(int minComp);
    int getMinComponents();

    void setMaxComponents(int maxComp);
    int getMaxComponents();

    void setConvolver(const std::shared_ptr<nsx::Imaging::Convolver>& convolver);
    std::shared_ptr<nsx::Imaging::Convolver> getConvolver();

    void setConvolutionKernel(const std::shared_ptr<nsx::Imaging::ConvolutionKernel>& kernel);

    void setKernel(const std::shared_ptr<nsx::Imaging::ConvolutionKernel>& kernel);
    std::shared_ptr<nsx::Imaging::ConvolutionKernel> getKernel();

private:
    std::shared_ptr<nsx::Utils::ProgressHandler> _handler;
    std::shared_ptr<nsx::Imaging::Convolver> _convolver;
    std::shared_ptr<nsx::Imaging::ConvolutionKernel> _kernel;

    double _thresholdValue;
    int _thresholdType;

    double _confidence;
    double _median;

    int _minComp;
    int _maxComp;
};

} // end namespace nsx

#endif // PEAKFINDER_H
