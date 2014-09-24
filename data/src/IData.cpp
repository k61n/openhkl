#include "IData.h"
namespace SX
{

namespace Data
{
IData::IData()
{
}

IData::IData(SX::Data::IDataReader* reader):_mm(reader)
{
}


IData::~IData()
{
}

}
}
