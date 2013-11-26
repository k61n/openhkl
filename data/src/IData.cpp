#include "IData.h"

namespace SX
{

namespace Data
{

template<typename T>
IData<T>::IData() : _meta(nullptr), _nFrames(0)
{
	_frames.reserve(0);
}

template<typename T>
IData<T>::~IData()
{
	delete _meta;
}

} // namespace Data

} // namespace SX
