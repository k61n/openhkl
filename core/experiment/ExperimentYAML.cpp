//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      base/parser/ExperimentYAML.cpp
//! @brief     Defines function eigenToVector
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "core/experiment/ExperimentYAML.h"

#include "base/utils/Logger.h"
#include "core/data/DataSet.h"
#include "core/data/DataTypes.h"
#include "core/loader/RawDataReader.h"
#include "core/loader/TiffDataReader.h"

#include <stdexcept>
#include <yaml-cpp/node/node.h>
#include <yaml-cpp/node/parse.h>

namespace ohkl {

ExperimentYAML::ExperimentYAML(const std::string& filename)
{
    _node = YAML::LoadFile(filename);

}

void ExperimentYAML::setDataReaderParameters(DataReaderParameters* params) const
{
    ohklLog(Level::Info, "ExperimentYAML::setDataReaderParameters: reading parameters from yml");
    if (!_node["DataReader"])
        throw std::runtime_error("No DataReader node in this yaml file");

    YAML::Node branch = _node["DataReader"];

    params->dataset_name = getNode<std::string>(branch, "name");
    params->wavelength = getNode<double>(branch, "wavelength");
    params->delta_chi = getNode<double>(branch, "delta_chi");
    params->delta_omega = getNode<double>(branch, "delta_omega");
    params->delta_phi = getNode<double>(branch, "delta_phi");
    params->bytes_per_pixel = getNode<int>(branch, "bytes_per_pixel");
    params->baseline = getNode<double>(branch, "baseline");
    params->gain = getNode<double>(branch, "gain");

    auto format = getNode<std::string>(branch, "format");
    if (format.empty())
        throw std::runtime_error("DataReader/format not specified");
    else {
        if (format == "raw")
            params->format = DataFormat::RAW;
        if (format== "tiff")
            params->format = DataFormat::TIFF;
    }

    if (format == "raw") {
        params->cols = getNode<int>(branch, "columns");
        params->rows = getNode<int>(branch, "rows");
        params->swap_endian = getNode<bool>(branch, "swap_endian");
        params->row_major = getNode<bool>(branch, "row_major");
    } else if (format == "tiff") {
        params->rebin_size = getNode<int>(branch, "rebin_size");
    }
}

} // namespace ohkl
