// author: Jonathan Fisher
// j.fisher@fz-juelich.de

#ifndef NSXTOOL_BASICFRAMEITERATOR_H_
#define NSXTOOL_BASICFRAMEITERATOR_H_

#include "IFrameIterator.h"
#include "Types.h"

namespace SX {
namespace Data {

class BasicFrameIterator final: public IFrameIterator {
public:
    BasicFrameIterator(IData& data, unsigned int idx);
    ~BasicFrameIterator() = default;
    SX::Types::RealMatrix& getFrame() override;
    void advance() override;
private:
    SX::Types::RealMatrix _currentFrame;
};

} // namespace Data
} // namespace SX

#endif // NSXTOOL_BASICFRAMEITERATOR_H_
