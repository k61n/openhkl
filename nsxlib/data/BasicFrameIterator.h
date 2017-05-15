// author: Jonathan Fisher
// j.fisher@fz-juelich.de

#ifndef NSXTOOL_BASICFRAMEITERATOR_H_
#define NSXTOOL_BASICFRAMEITERATOR_H_

#include "IFrameIterator.h"
#include "../utils/Types.h"

namespace nsx {
namespace Data {

class BasicFrameIterator final: public IFrameIterator {
public:
    BasicFrameIterator(DataSet& data, unsigned int idx);
    ~BasicFrameIterator() = default;
    nsx::Types::RealMatrix& getFrame() override;
    void advance() override;
private:
    nsx::Types::RealMatrix _currentFrame;
};

} // namespace Data
} // end namespace nsx

#endif // NSXTOOL_BASICFRAMEITERATOR_H_
