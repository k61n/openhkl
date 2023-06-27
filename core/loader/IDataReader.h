//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/loader/IDataReader.h
//! @brief     Defines class IDataReader
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_CORE_RAW_IDATAREADER_H
#define OHKL_CORE_RAW_IDATAREADER_H

#include "core/data/DataTypes.h"
#include "core/instrument/InstrumentState.h"
#include "core/instrument/InstrumentStateSet.h"
#include "core/raw/MetaData.h"

#include <Eigen/Dense>
#include <string>
#include <vector>

namespace ohkl {

//! Pure virtual base class of data readers that provide access to detector images and metadata.

class InstrumentStateSet;
class DataSet;

//! Minimal meta data set
struct DataReaderParameters {
    DataFormat format = DataFormat::Unknown;
    int rows = -1;
    int cols = -1;
    std::string dataset_name = kw_datasetDefaultName;
    double wavelength = 0.0;
    double delta_omega = 0.0;
    double delta_chi = 0.0;
    double delta_phi = 0.0;
    bool swap_endian = true;
    double baseline = 0.0;
    double gain = 1.0;
    std::size_t bytes_per_pixel = 2;
    bool row_major = true; // raw
    int rebin_size = 1; // tiff

    void log(const Level& level) const;
    void loadFromYAML(std::string file);
};

class IDataReader {

 public:
    //! Status of reader initialization
    bool isInitialized = false;

    IDataReader(const std::string& filename);
    virtual ~IDataReader() = 0;

    //! Initialize reading from the filename
    virtual bool initRead();

    //! Open the file
    virtual void open() = 0;

    //! Close file and release handle
    virtual void close() = 0;

    //! Returns the a detector image
    virtual Eigen::MatrixXi data(size_t frame) = 0;

    //! True if file is open
    bool isOpened() const;

    //! Set the DataSet where the data will be stored (allowed only once)
    void setDataSet(DataSet* dataset_out);

    //! Check if the reader is successfully initialized
    void checkInit();

    //! Return the NSX (HDF5) filepath associated with the dataset (if any)
    virtual std::string OHKLfilepath() const;

 protected:
    IDataReader() = delete;

    IDataReader(const IDataReader& other) = delete;

    IDataReader& operator=(const IDataReader& other) = delete;

    //! Status of file handle
    bool _isOpened;

    //! Filename
    std::string _filename;

    //! Destination DataSet where the data will be stored
    DataSet* _dataset_out = nullptr;
};

} // namespace ohkl

#endif // OHKL_CORE_RAW_IDATAREADER_H
