// author: Jonathan Fisher
// j.fisher@fz-juelich.de

#ifndef NSXLIB_THREADEDFRAMEITERATOR_H
#define NSXLIB_THREADEDFRAMEITERATOR_H

#include "IData.h"
#include "IFrameIterator.h"
#include "../utils/Types.h"

#include <future>

namespace nsx {

class ThreadedFrameIterator: public IFrameIterator {
public:
    ThreadedFrameIterator(DataSet& data, unsigned int idx);
    ~ThreadedFrameIterator() = default;
    RealMatrix& getFrame() override;
    void advance() override;

private:
    RealMatrix _currentFrame;
    std::shared_future<RealMatrix> _nextFrame;
    std::shared_future<RealMatrix> getFrameAsync(int idx);
};

} // end namespace nsx

#endif // NSXLIB_THREADEDFRAMEITERATOR_H
