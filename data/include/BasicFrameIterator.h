// author: Jonathan Fisher
// j.fisher@fz-juelich.de

#ifndef NSXTOOL_BASICFRAMEITERATOR_H_
#define NSXTOOL_BASICFRAMEITERATOR_H_

#include "IFrameIterator.h"
#include "Types.h"


namespace SX {

namespace Data {

class BasicFrameIterator: public IFrameIterator {
public:
    BasicFrameIterator(IData* data, int idx);
    ~BasicFrameIterator();

    SX::Types::RealMatrix& getFrame() override;
    void advance() override;
    int index() override;

private:
    int _index;
    IData* _data;
    SX::Types::RealMatrix _currentFrame;

};

} // namespace Data


} // namespace SX


#endif // NSXTOOL_BASICFRAMEITERATOR_H_
