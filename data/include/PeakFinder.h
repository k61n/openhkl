
#ifndef NSXTOOL_PEAKFINDER_H_
#define NSXTOOL_PEAKFINDER_H_

#include <memory>

namespace SX {

namespace Imaging {
class Convolver;
}

namespace Data {

class IData;

class PeakFinder
{
public:
    PeakFinder();
    void find(std::vector<IData*> numors, double threshold, double confidence, int minComp, int maxComp, std::shared_ptr<SX::Imaging::Convolver> convolver);
};

} // namespace Data

} // namespace SX

#endif // PEAKFINDER_H
