// author: Jonathan Fisher
// j.fisher@fz-juelich.de

#ifndef NSXTOOL_BASICFRAMEITERATOR_H_
#define NSXTOOL_BASICFRAMEITERATOR_H_

#include "IFrameIterator.h"
#include "../utils/Types.h"

namespace nsx {

class BasicFrameIterator final: public IFrameIterator {
public:
    BasicFrameIterator(DataSet& data, unsigned int idx);
    ~BasicFrameIterator() = default;
    RealMatrix& getFrame() override;
    void advance() override;
private:
    RealMatrix _currentFrame;
};

} // end namespace nsx

#endif // NSXTOOL_BASICFRAMEITERATOR_H_
