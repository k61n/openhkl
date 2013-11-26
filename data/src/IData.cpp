#include "IData.h"

namespace SX
{

namespace Data
{

IData::IData() : _meta(nullptr)
{
}

IData::~IData()
{
	delete _meta;
}

} // namespace Data

} // namespace SX
