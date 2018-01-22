#pragma once

#include <future>

#include "DataTypes.h"
#include "IFrameIterator.h"
#include "MathematicsTypes.h"

namespace nsx {

class ThreadedFrameIterator: public IFrameIterator {
public:
    ThreadedFrameIterator(DataSet& data, unsigned int idx);
    ~ThreadedFrameIterator() = default;
    RealMatrix& frame() override;
    void advance() override;

private:
    RealMatrix _currentFrame;
    std::shared_future<RealMatrix> _nextFrame;
    std::shared_future<RealMatrix> getFrameAsync(int idx);
};

} // end namespace nsx
