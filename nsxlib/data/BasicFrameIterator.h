#pragma once

#include "DataTypes.h"
#include "IFrameIterator.h"
#include "MathematicsTypes.h"

namespace nsx {

class BasicFrameIterator: public IFrameIterator {
public:
    BasicFrameIterator(DataSet& data, unsigned int idx);
    ~BasicFrameIterator() = default;
    RealMatrix& getFrame() override;
    void advance() override;
private:
    RealMatrix _currentFrame;
};

} // end namespace nsx
