//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/raw/PckDataReader.h
//! @brief     Defines class PckDataReader
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_CORE_LOADER_PCKDATAREADER_H
#define OHKL_CORE_LOADER_PCKDATAREADER_H

#include "base/utils/Logger.h"
#include "core/loader/IDataReader.h"
#include "core/loader/RawDataReader.h"
#include "core/raw/DataKeys.h"

#include "tiffio.h"
#include <filesystem>
#include <string>
#include <vector>

namespace ohkl {

class PckDataReader : public IDataReader {
 public:
    //! Constructor
    PckDataReader();
    //! Constructor
    PckDataReader(const PckDataReader& other) = delete;
    //! Destructor
    ~PckDataReader() = default;
    //! Open method from abstract base class
    void open() override{};
    //! Close method from abstract base class
    void close() override{};
    //! add a frame/file to the PckDataReader
    void addFrame(const std::string& filename);
    //! initRead
    bool initRead() override;
    //! access data
    Eigen::MatrixXi data(size_t frame) override final;
    //! set Parameters
    void setParameters(const DataReaderParameters& parameters);

 private:
    std::vector<std::string> _filenames;
    DataReaderParameters _parameters;

    void readMar(const std::string& filename);
};

} // namespace ohkl
#endif // OHKL_CORE_LOADER_PCKDATAREADER_H
