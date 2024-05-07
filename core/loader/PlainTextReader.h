//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/loader/PlainTextReader.h
//! @brief     Defines class PlainTextReader
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_CORE_LOADER_PLAINTEXTREADER_H
#define OHKL_CORE_LOADER_PLAINTEXTREADER_H

#include "core/data/DataSet.h"
#include "core/loader/IDataReader.h" // inherits from
#include "core/raw/DataKeys.h"

#include <fstream>
#include <stdexcept>

namespace ohkl {

//! Class for reading binary raw image files
class PlainTextReader : public IDataReader {
 public:
    PlainTextReader(const PlainTextReader& other) = delete;

    PlainTextReader();

    ~PlainTextReader() = default;

    void addFrame(const std::string& filename);

    //! Open the file(s)
    void open() final;

    //! Close the file(s)
    void close() final;

    virtual bool initRead() override;

    //! Read a single frame
    Eigen::MatrixXi data(size_t frame) final;

    const DataReaderParameters& parameters() const;
    void setParameters(const DataReaderParameters& parameters);

 private:
    template <typename T_> Eigen::Matrix<T_, Eigen::Dynamic, Eigen::Dynamic> matrixFromData() const;

 private:
    std::vector<std::string> _filenames;

    DataReaderParameters _parameters;
};

} // namespace ohkl

#endif // OHKL_CORE_LOADER_PLAINTEXTREADER_H
