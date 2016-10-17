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
	// Files with no extensions are legacy ILL ASCII
    registerCallback("" ,    &ILLAsciiData::create);

	// HDF5 format, register recommended extensions
    registerCallback("h5",   &HDF5Data::create);
    registerCallback("hdf5", &HDF5Data::create);
    registerCallback("hdf",  &HDF5Data::create);
    registerCallback("nxs",  &HDF5Data::create);

	// TIFF data
    registerCallback("tiff", &TiffData::create);
    registerCallback("tif",  &TiffData::create);

    // Register dat extension for I16 data
    registerCallback("dat",  &I16Data::create);

    // Register raw extension for raw data
    registerCallback("raw",  &RawData::create);
}

DataReaderFactory::~DataReaderFactory()
{
}

} // end namespace Data

} // end namespace SX


