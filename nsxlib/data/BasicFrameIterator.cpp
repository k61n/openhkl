// author: Jonathan Fisher
// j.fisher@fz-juelich.de

#include "IFrameIterator.h"
#include "BasicFrameIterator.h"
#include "IData.h"

namespace nsx {
namespace Data {

BasicFrameIterator::BasicFrameIterator(DataSet& data, unsigned int idx): IFrameIterator(data, idx)
{
    _currentFrame = _data.getFrame(_index).cast<double>();
}

nsx::Types::RealMatrix& BasicFrameIterator::getFrame()
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

} // namespace Data
} // namespace nsx
