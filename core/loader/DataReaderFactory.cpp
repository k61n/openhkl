#include <stdexcept>

#include "DataReaderFactory.h"
#include "DataSet.h"
#include "FakeDataReader.h"
#include "HDF5DataReader.h"
#include "ILLDataReader.h"
#include "RawDataReader.h"
#include "TiffDataReader.h"

namespace nsx {

template <typename T> 
IDataReader* create_reader(const std::string& filename, Diffractometer *diffractometer)
{
    return new T(filename, diffractometer);
}

DataReaderFactory::DataReaderFactory(): _callbacks()
{
    _callbacks[""] = &create_reader<ILLDataReader>;
    _callbacks["fake"] = &create_reader<FakeDataReader>;
    _callbacks["h5"] = &create_reader<HDF5DataReader>;
    _callbacks["hdf5"] = &create_reader<HDF5DataReader>;
    _callbacks["hdf"] = &create_reader<HDF5DataReader>;                                   
    _callbacks["nxs"] = &create_reader<HDF5DataReader>;
    _callbacks["raw"] = &create_reader<RawDataReader>;
    _callbacks["tif"] = &create_reader<TiffDataReader>;
    _callbacks["tiff"] = &create_reader<TiffDataReader>;
}

IDataReader* DataReaderFactory::create(const std::string& extension, const std::string& filename, Diffractometer *diffractometer) const
{
    const auto it = _callbacks.find(extension);

    // could not find key
    if (it == _callbacks.end()) {
        throw std::runtime_error("could not find given extension in map of callbacks");
    }

    return (it->second)(filename, diffractometer);
}

} // end namespace nsx
