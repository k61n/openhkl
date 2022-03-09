//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/raw/IDataReader.cpp
//! @brief     Implements class IDataReader
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "core/loader/IDataReader.h"
#include "core/data/DataSet.h"
#include "core/instrument/Diffractometer.h"
#include "core/raw/DataKeys.h"

#include <cassert>
#include <stdexcept>

namespace nsx {

IDataReader::IDataReader(const std::string& filename) : _isOpened(false), _filename(filename) { }

IDataReader::~IDataReader() = default;

bool IDataReader::initRead()
{

    if (!_dataset_out) {
        throw std::runtime_error("RawDataReader: No DataSet available for output.");
    }

    if (!_dataset_out->diffractometer()) {
        throw std::runtime_error("RawDataReader: No Diffractometer available.");
    }

    // Ensure that there is at least one monochromator
    if (_dataset_out->diffractometer()->source().nMonochromators() == 0) {
        Monochromator mono(nsx::kw_monochromatorDefaultName);
        _dataset_out->diffractometer()->source().addMonochromator(mono);
        nsxlog(
            Level::Warning, __FUNCTION__,
            ": Source had no monochromators; hence, a default monochromator is created.");
    }

    return true;
}

bool IDataReader::isOpened() const
{
    return _isOpened;
}

void IDataReader::setDataSet(DataSet* dataset_ptr)
{
    // Disallow resetting the destination DataSet
    if (!_dataset_out) {
        if (dataset_ptr)
            _dataset_out = dataset_ptr;
        else // Disallow setting the destination DataSet to a null pointer
            throw std::invalid_argument("DataReader: Cannot set the destination DataSet to null");
    } else
        throw std::invalid_argument("DataReader: Cannot reset the destination DataSet");
}

void IDataReader::checkInit()
{
    if (!isInitialized)
        throw std::runtime_error("Reader is not initialized");
}

std::string IDataReader::NSXfilepath() const
{
    return "";
}

} // namespace nsx
