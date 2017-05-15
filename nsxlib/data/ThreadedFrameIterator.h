// author: Jonathan Fisher
// j.fisher@fz-juelich.de

#ifndef NSXTOOL_THREADEDFRAMEITERATOR_H_
#define NSXTOOL_THREADEDFRAMEITERATOR_H_

#include "IData.h"
#include "IFrameIterator.h"
#include "../utils/Types.h"

#include <future>

namespace nsx {

namespace Data {

class ThreadedFrameIterator final: public IFrameIterator {
public:
    ThreadedFrameIterator(DataSet& data, unsigned int idx);
    ~ThreadedFrameIterator() = default;
    nsx::Types::RealMatrix& getFrame() override;
    void advance() override;

private:
    nsx::Types::RealMatrix _currentFrame;
    std::shared_future<nsx::Types::RealMatrix> _nextFrame;
    std::shared_future<nsx::Types::RealMatrix> getFrameAsync(int idx);
};

} // Data

} // SX

#endif // NSXTOOL_THREADEDFRAMEITERATOR_H_
