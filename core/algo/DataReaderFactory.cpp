//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/algo/DataReaderFactory.cpp
//! @brief     Implements class DataReaderFactory
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "core/algo/DataReaderFactory.h"

#include "core/loader/FakeDataReader.h"
#include "core/loader/HDF5DataReader.h"
#include "core/loader/NexusDataReader.h"
#include "core/loader/RawDataReader.h"

#include <stdexcept>

namespace nsx {

template <typename T>
std::shared_ptr<DataSet> create_reader(const std::string& filename, Diffractometer* diffractometer)
{
    return std::make_shared<DataSet>(std::shared_ptr<IDataReader>{new T(filename, diffractometer)});
}

DataReaderFactory::DataReaderFactory() : _callbacks()
{
    _callbacks["fake"] = &create_reader<FakeDataReader>;
    _callbacks["hdf"] = &create_reader<HDF5DataReader>;  // TODO: Warn for deprecation
    _callbacks["nsx"] = &create_reader<HDF5DataReader>;
    _callbacks["nxs"] = &create_reader<NexusDataReader>;
    _callbacks["raw"] = &create_reader<RawDataReader>;
}

// TODO: DataReaderFactory makes DataSet not an IDataReader!

std::shared_ptr<DataSet> DataReaderFactory::create(
    const std::string& extension, const std::string& filename, Diffractometer* diffractometer) const
{
    const auto it = _callbacks.find(extension);

    // could not find key
    if (it == _callbacks.end())
        throw std::runtime_error("could not find given extension in map of callbacks");

    return (it->second)(filename, diffractometer);
}

} // namespace nsx
