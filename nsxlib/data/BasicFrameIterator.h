// author: Jonathan Fisher
// j.fisher@fz-juelich.de

#ifndef NSXLIB_BASICFRAMEITERATOR_H
#define NSXLIB_BASICFRAMEITERATOR_H

#include "IFrameIterator.h"
#include "../data/DataTypes.h"
#include "../mathematics/MathematicsTypes.h"

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

#endif // NSXLIB_BASICFRAMEITERATOR_H
