//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/loader/ExperimentDataReader.h
//! @brief     Defines class ExperimentDataReader
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef NSX_CORE_LOADER_EXPERIMENTDATAREADER_H
#define NSX_CORE_LOADER_EXPERIMENTDATAREADER_H

#include "core/loader/ExperimentMetaReader.h" // inherits from

namespace nsx {

//! Read data from HDF5 format.

class ExperimentDataReader : public ExperimentMetaReader {
 public:
    ExperimentDataReader() = delete;

    ExperimentDataReader(const ExperimentDataReader& other) = delete;

    // Note that we need this constructor explicitly defined for SWIG.
    ExperimentDataReader(
        const std::string& file_name, const std::string& group_name, Diffractometer* instrument);

    ~ExperimentDataReader() = default;

    ExperimentDataReader& operator=(const ExperimentDataReader& other) = delete;

    Eigen::MatrixXi data(size_t frame) final;
};

} // namespace nsx

#endif // NSX_CORE_LOADER_EXPERIMENTDATAREADER_H
