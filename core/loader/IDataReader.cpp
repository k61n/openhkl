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

#include "base/utils/Path.h" // fileBasename
#include "core/data/DataSet.h"
#include "core/instrument/Diffractometer.h"
#include "core/raw/DataKeys.h"

#include <cassert>
#include <stdexcept>

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
}

void DataReaderParameters::LoadDataFromFile(std::string file)
{
    dataset_name = fileBasename(file);

    std::size_t pos1 = file.find_last_of("/");
    std::size_t pos0 = (file.substr(0, pos1 - 1)).find_last_of("/");
    std::size_t pos2 = file.find_last_of(".");

    if (pos1 == std::string::npos || pos0 == std::string::npos || pos2 == std::string::npos)
        return;

    std::string dir = "data_" + file.substr(pos0 + 1, pos1 - pos0 - 1);
    std::string readme = file.substr(0, pos1 + 1) + dir + ".readme";

    std::ifstream fin(readme.c_str(), std::ios::in);

    if (fin.is_open() || fin.good()) {
        fin.seekg(0, std::ios::end);
        auto fsize = fin.tellg();
        fin.seekg(0, std::ios::beg);

        if (fsize > 0) {
            std::string buffer;
            buffer.resize(fsize);
            fin.read(buffer.data(), fsize);
            fin.close();

            std::remove_if(buffer.begin(), buffer.end(), isspace);

            auto omega_pos = buffer.find("Omegarange:");
            if (omega_pos != std::string::npos) {
                auto nl_pos = buffer.find(";", omega_pos);
                std::string a = buffer.substr(omega_pos + 11, nl_pos - 1);
                delta_omega = std::stod(a);
            }

            auto lambda_pos = buffer.find("Lambda:");
            if (lambda_pos != std::string::npos) {
                auto nl_pos = buffer.find(";", lambda_pos);
                std::string b = buffer.substr(lambda_pos + 7, nl_pos - 1);
                wavelength = std::stod(b);
            }
        }
    }
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

std::string IDataReader::OHKLfilepath() const
{
    return "";
}

} // namespace ohkl
