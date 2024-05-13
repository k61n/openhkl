//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/raw/IDataReader.cpp
//! @brief     Implements class IDataReader
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "core/loader/IDataReader.h"

#include "base/utils/Logger.h"
#include "core/data/DataSet.h"
#include "core/experiment/ExperimentYAML.h"
#include "core/instrument/Diffractometer.h"
#include "core/raw/DataKeys.h"

namespace ohkl {

void DataReaderParameters::log(const Level& level) const
{
    ohklLog(level, "DataReaderParameters::log:");
    ohklLog(level, "wavelength      = ", wavelength);
    ohklLog(level, "delta_omega     = ", delta_omega);
    ohklLog(level, "delta_chi       = ", delta_chi);
    ohklLog(level, "delta_phi       = ", delta_phi);
    ohklLog(level, "swap_endian     = ", swap_endian);
    ohklLog(level, "baseline        = ", baseline);
    ohklLog(level, "gain            = ", gain);
    ohklLog(level, "bytes_per_pixel = ", bytes_per_pixel);
    if (data_format == DataFormat::RAW)
        ohklLog(level, "row_major       = ", row_major);
    if (data_format == DataFormat::TIFF)
        ohklLog(level, "rebin_size      = ", rebin_size);
}

void DataReaderParameters::loadFromYAML(std::string file)
{
    ohklLog(Level::Info, "DataReaderParameters::loadFromYAML");
    ExperimentYAML reader(file);
    reader.grabDataReaderParameters(this);
}

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
        Monochromator mono(ohkl::kw_monochromatorDefaultName);
        _dataset_out->diffractometer()->source().addMonochromator(mono);
        ohklLog(
            Level::Warning, __FUNCTION__,
            ": Source had no monochromator. A default monochromator was created.");
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

std::string IDataReader::OHKLfilepath() const
{
    return "";
}

} // namespace ohkl
