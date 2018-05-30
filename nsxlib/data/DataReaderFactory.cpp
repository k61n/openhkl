#include <stdexcept>

#include "DataReaderFactory.h"
#include "DataSet.h"
#include "HDF5DataReader.h"
#include "I16DataReader.h"
#include "ILLDataReader.h"
#include "RawDataReader.h"
#include "TiffDataReader.h"

namespace nsx {

template <typename T> 
std::shared_ptr<DataSet> create_reader(const std::string& filename, const std::shared_ptr<Diffractometer>& diff)
{
    auto reader = std::shared_ptr<IDataReader>(new T(filename, diff));
    return std::shared_ptr<DataSet>(new DataSet(reader));
}

DataReaderFactory::DataReaderFactory(): _callbacks()
{
    _callbacks[""] = &create_reader<ILLDataReader>; // Files with no extensions are legacy ILL ASCII
    _callbacks["h5"] = &create_reader<HDF5DataReader>;
    _callbacks["hdf5"] = &create_reader<HDF5DataReader>;
    _callbacks["hdf"] = &create_reader<HDF5DataReader>;                                   
    _callbacks["nxs"] = &create_reader<HDF5DataReader>;
    _callbacks["tif"] = &create_reader<TiffDataReader>;
    _callbacks["tiff"] = &create_reader<TiffDataReader>;
    //_callbacks["raw"] = &create_reader<RawDataReader>;
}

std::shared_ptr<DataSet> DataReaderFactory::create(const std::string& extension, const std::string& filename, const std::shared_ptr<Diffractometer>& diffractometer) const
{
    const auto it = _callbacks.find(extension);

    // could not find key
    if (it == _callbacks.end()) {
        throw std::runtime_error("could not find given extension in map of callbacks");
    }

    return (it->second)(filename, diffractometer);
}

} // end namespace nsx
