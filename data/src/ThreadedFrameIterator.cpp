#include "IFrameIterator.h"
#include "ThreadedFrameIterator.h"


namespace SX {

namespace Data {
 /*

ThreadedFrameIterator::ThreadedFrameIterator(IData* parent, int idx, std::launch policy)
    :_currentFrame(idx),
     _parent(parent),
     _currentData(),
     _nextData(),
     _launchPolicy(policy)
{
    if (_currentFrame < _parent->_nFrames)
        _currentData = _parent->getFrame(_currentFrame);

    if ( _currentFrame+1 < _parent->_nFrames )
        _nextData = getFrame(_currentFrame+1);
}

ThreadedFrameIterator::ThreadedFrameIterator(const IData::ThreadedFrameIterator& other)
    :_currentFrame(other._currentFrame),
     _parent(other._parent),
     _currentData(other._currentData),
     _nextData(other._nextData),
     _launchPolicy(other._launchPolicy)
{
}

IData::ThreadedFrameIterator& IData::ThreadedFrameIterator::operator++()
{
    assert(_currentFrame != _parent->_nFrames);

    ++_currentFrame;

    if ( _currentFrame < _parent->_nFrames) {
        _currentData = _nextData.get();
        _nextData = getFrame(_currentFrame);
    }

    return *this;
}

std::shared_future<Eigen::MatrixXi> IData::ThreadedFrameIterator::getFrame(int idx)
{
    return std::shared_future<Eigen::MatrixXi>(std::async(_launchPolicy, [=]{return _parent->getFrame(idx);}));
}

Eigen::MatrixXi& IData::ThreadedFrameIterator::operator*()
{
    return _currentData;
}

Eigen::MatrixXi* IData::ThreadedFrameIterator::operator->()
{
    return &_currentData;
}

bool IData::ThreadedFrameIterator::operator!=(const IData::ThreadedFrameIterator& other) const
{
    if (_parent != other._parent)
        return true;

    if ( _currentFrame != other._currentFrame)
        return true;

    return false;
}

bool IData::ThreadedFrameIterator::operator==(const IData::ThreadedFrameIterator& other) const
{
    return !(*this != other);
}


ThreadedFrameIterator IData::begin()
{
    return at(0);
}

ThreadedFrameIterator IData::end()
{
    return at(_nFrames);
}

ThreadedFrameIterator IData::at(int idx)
{
    return ThreadedFrameIterator(this, idx, _inMemory ? std::launch::deferred : std::launch::async);
}

*/

} // namespace Data

} // namespace SX
