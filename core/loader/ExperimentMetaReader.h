//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/loader/ExperimentMetaReader.h
//! @brief     Defines class ExperimentMetaReader
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef NSX_CORE_LOADER_EXPERIMENTMETAREADER_H
#define NSX_CORE_LOADER_EXPERIMENTMETAREADER_H

#include "core/raw/IDataReader.h" // inherits from
#include <H5Cpp.h>

namespace nsx {

class ExperimentMetaReader : public IDataReader {
 public:
    ExperimentMetaReader() = delete;

    ExperimentMetaReader(const ExperimentMetaReader& other) = delete;

    ExperimentMetaReader(
        const std::string& file_name, const std::string& group_name, Diffractometer* diffractometer);

    ~ExperimentMetaReader() override = default;

    ExperimentMetaReader& operator=(const ExperimentMetaReader& other) = delete;

 public:
    //! override open of the file
    virtual void open() override;
    //! override close of the file
    virtual void close() override;

 protected:
    std::unique_ptr<H5::H5File> _file;
    std::unique_ptr<H5::Group> _group;
    std::unique_ptr<H5::DataSet> _dataset;
    std::unique_ptr<H5::DataSpace> _space;
    std::unique_ptr<H5::DataSpace> _memspace;
};

} // namespace nsx

#endif // NSX_CORE_LOADER_EXPERIMENTMETAREADER_H
