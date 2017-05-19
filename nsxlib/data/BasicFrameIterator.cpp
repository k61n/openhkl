// author: Jonathan Fisher
// j.fisher@fz-juelich.de

#include "../data/BasicFrameIterator.h"
#include "../data/DataSet.h"
#include "../data/IFrameIterator.h"

namespace nsx {

BasicFrameIterator::BasicFrameIterator(DataSet& data, unsigned int idx): IFrameIterator(data, idx)
{
    _currentFrame = _data.getFrame(_index).cast<double>();
}

RealMatrix& BasicFrameIterator::getFrame()
{
    return _currentFrame;
}

void BasicFrameIterator::advance()
{
    ++_index;
    if (_index != _data.getNFrames()) {
        _currentFrame = _data.getFrame(_index).cast<double>();
    }
}

} // end namespace nsx
