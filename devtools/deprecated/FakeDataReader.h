//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/loader/FakeDataReader.h
//! @brief     Defines class FakeDataReader
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef NSX_CORE_LOADER_FAKEDATAREADER_H
#define NSX_CORE_LOADER_FAKEDATAREADER_H

#include "core/loader/HDF5MetaDataReader.h"

namespace nsx {

//! Makes simulated data behave as if it were real. Used for testing purposes.

class FakeDataReader : public HDF5MetaDataReader {
 public:
    FakeDataReader() = delete;

    FakeDataReader(const FakeDataReader& other) = delete;

    //! Constructor. Behaves as if it is loaded from filename (which must exist!),
    //! but the frame values may be overwritten arbitrarily (without modifying the
    //! file on disk).
    FakeDataReader(const std::string& filename, Diffractometer* diffractometer);

    ~FakeDataReader() = default;

    FakeDataReader& operator=(const FakeDataReader& other) = delete;

    //! Returns the data frame for the given index.
    Eigen::MatrixXi data(size_t frame) final;

    //! Sets the value of the data at the given frame
    void setData(size_t frame, const Eigen::MatrixXi& data);

 private:
    //! Stored data frames. Could be real or simulated/faked.
    std::vector<Eigen::MatrixXi> _frames;
};

} // namespace nsx

#endif // NSX_CORE_LOADER_FAKEDATAREADER_H
