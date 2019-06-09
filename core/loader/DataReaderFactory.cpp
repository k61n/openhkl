//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/loader/DataReaderFactory.cpp
//! @brief     Implements ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include <stdexcept>

#include "core/experiment/DataSet.h"
#include "core/loader/DataReaderFactory.h"
#include "core/loader/FakeDataReader.h"
#include "core/loader/HDF5DataReader.h"
#include "core/loader/ILLDataReader.h"
#include "core/loader/RawDataReader.h"
#include "core/loader/TiffDataReader.h"

namespace nsx {

template <typename T>
std::shared_ptr<DataSet> create_reader(const std::string& filename, Diffractometer* diffractometer)
{
    auto reader = std::shared_ptr<IDataReader>(new T(filename, diffractometer));
    return std::shared_ptr<DataSet>(new DataSet(reader));
}

DataReaderFactory::DataReaderFactory() : _callbacks()
{
    _callbacks[""] = &create_reader<ILLDataReader>; // Files with no extensions
                                                    // are legacy ILL ASCII
    _callbacks["fake"] = &create_reader<FakeDataReader>;
    _callbacks["h5"] = &create_reader<HDF5DataReader>;
    _callbacks["hdf5"] = &create_reader<HDF5DataReader>;
    _callbacks["hdf"] = &create_reader<HDF5DataReader>;
    _callbacks["nxs"] = &create_reader<HDF5DataReader>;
    _callbacks["raw"] = &create_reader<RawDataReader>;
    _callbacks["tif"] = &create_reader<TiffDataReader>;
    _callbacks["tiff"] = &create_reader<TiffDataReader>;
}

std::shared_ptr<DataSet> DataReaderFactory::create(
    const std::string& extension, const std::string& filename, Diffractometer* diffractometer) const
{
    const auto it = _callbacks.find(extension);

    // could not find key
    if (it == _callbacks.end()) {
        throw std::runtime_error("could not find given extension in map of callbacks");
    }

    return (it->second)(filename, diffractometer);
}

} // namespace nsx
