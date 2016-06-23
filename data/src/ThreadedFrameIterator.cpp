#include "IFrameIterator.h"
#include "ThreadedFrameIterator.h"
#include "IData.h"

#include <future>



namespace SX {

namespace Data {



ThreadedFrameIterator::ThreadedFrameIterator(IData* data, int idx)
    :IFrameIterator(data, idx),
     _currentFrame(),
     _nextFrame()
{
    std::launch policy = std::launch::async;
    int nframes = _data->getNFrames();

    if (_index < nframes)
        _currentFrame = _data->getFrame(_index).cast<double>();

    if ( _index+1 < nframes )
        _nextFrame = getFrameAsync(_index+1);
}

ThreadedFrameIterator::~ThreadedFrameIterator()
{

}

Types::RealMatrix &ThreadedFrameIterator::getFrame()
{
    assert(_index < _data->getNFrames() );
    return _currentFrame;
}


void ThreadedFrameIterator::advance()
{
    int nframes = _data->getNFrames();
    ++_index;

    if (_index < nframes)
        _currentFrame = _nextFrame.get();

    if (_index+1 < nframes)
        _nextFrame = getFrameAsync(_index+1);
}

std::shared_future<SX::Types::RealMatrix> ThreadedFrameIterator::getFrameAsync(int idx)
{
    std::launch policy = std::launch::async;

    auto get_fn = [=] () -> SX::Types::RealMatrix
    {
        return _data->getFrame(idx).cast<double>();
    };

    return std::shared_future<SX::Types::RealMatrix>(std::async(policy, get_fn));
}

/**/

} // namespace Data

} // namespace SX
