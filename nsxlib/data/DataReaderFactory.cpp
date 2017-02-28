#include <string>
#include "DataReaderFactory.h"
#include "ILLDataReader.h"
#include "HDF5DataReader.h"
#include "TiffDataReader.h"
#include "I16DataReader.h"
#include "RawDataReader.h"

namespace SX {
namespace Data {

template <typename Reader>
class C {
public:
static IData* create(const std::string& filename, const std::shared_ptr<Diffractometer>& diffractometer)
{
    auto reader = new Reader(filename, diffractometer);
    return new IData(reader, diffractometer);
}
};

DataReaderFactory::DataReaderFactory()
{
    registerCallback("" ,    &C<ILLDataReader>::create); // Files with no extensions are legacy ILL ASCII
    registerCallback("h5",   &C<HDF5DataReader>::create);
    registerCallback("hdf5", &C<HDF5DataReader>::create);
    registerCallback("hdf",  &C<HDF5DataReader>::create);
    registerCallback("nxs",  &C<HDF5DataReader>::create);
    registerCallback("tiff", &C<TiffDataReader>::create);
    registerCallback("tif",  &C<TiffDataReader>::create);
    registerCallback("dat",  &C<I16DataReader>::create);
    //registerCallback("raw",  &C<RawDataReader>::create);
}

} // end namespace Data
} // end namespace SX
