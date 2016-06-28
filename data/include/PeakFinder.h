
#ifndef NSXTOOL_PEAKFINDER_H_
#define NSXTOOL_PEAKFINDER_H_

#include <memory>


namespace SX {

namespace Imaging {
class Convolver;
}

namespace Utils {
class ProgressHandler;
}

namespace Data {

class IData;

class PeakFinder
{
public:
    PeakFinder();
    void find(std::vector<std::shared_ptr<IData>> numors,
              double threshold, double confidence, int minComp, int maxComp,
              std::shared_ptr<SX::Imaging::Convolver> convolver);

    void setHandler(std::shared_ptr<SX::Utils::ProgressHandler> handler);

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

    void setFilter();
    void getFilter();

private:
    std::shared_ptr<SX::Utils::ProgressHandler> _handler;
};

} // namespace Data

} // namespace SX

#endif // PEAKFINDER_H
