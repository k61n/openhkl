#include "BasicFrameIterator.h"
#include "DataSet.h"
#include "IFrameIterator.h"

namespace nsx {

BasicFrameIterator::BasicFrameIterator(DataSet& data, unsigned int idx): IFrameIterator(data, idx)
{
    _currentFrame = _data.frame(_index).cast<double>();
}

RealMatrix& BasicFrameIterator::frame()
{
    return _currentFrame;
}

void BasicFrameIterator::advance()
{
    ++_index;
    if (_index != _data.nFrames()) {
        _currentFrame = _data.frame(_index).cast<double>();
    }
}

} // end namespace nsx
