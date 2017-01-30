#include <string>
#include "DataReaderFactory.h"
#include "ILLAsciiData.h"
#include "HDF5Data.h"
#include "TiffData.h"
#include "I16Data.h"
#include "RawData.h"

namespace SX
{

namespace Data
{

DataReaderFactory::DataReaderFactory()
{
    registerCallback("" ,    &ILLAsciiData::create); // Files with no extensions are legacy ILL ASCII
    registerCallback("h5",   &HDF5Data::create);
    registerCallback("hdf5", &HDF5Data::create);
    registerCallback("hdf",  &HDF5Data::create);
    registerCallback("nxs",  &HDF5Data::create);
    registerCallback("tiff", &TiffData::create);
    registerCallback("tif",  &TiffData::create);
    registerCallback("dat",  &I16Data::create);
    registerCallback("raw",  &RawData::create);
}

} // end namespace Data
} // end namespace SX
