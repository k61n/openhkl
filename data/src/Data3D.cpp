#include <string>

#include "Data3D.h"
#include "MMILLAsciiReader.h"

namespace SX
{

namespace Data
{

Data3D::Data3D() : IData()
{
}

Data3D::~Data3D()
{
}

void Data3D::read(const std::string& filename)
{

    SX::MMILLAsciiReader mm(filename.c_str());
    _meta=mm.readMetaDataBlock();

}

} // namespace Data

} // namespace SX
