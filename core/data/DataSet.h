//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/data/DataSet.h
//! @brief     Defines class DataSet
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef NSX_CORE_DATA_DATASET_H
#define NSX_CORE_DATA_DATASET_H

#include "base/mask/IMask.h"
#include "core/instrument/InstrumentStateList.h"
#include "core/peak/Peak3D.h"
#include "core/raw/DataKeys.h"
#include "core/raw/IDataReader.h"
#include "core/data/DataTypes.h"


namespace nsx {

class Detector;
class DetectorEvent;

//! Class used to manage loading detector images and metadata from disk.
//!
//! Note that this class does not contain any actual data,
//! just a few metadata and a link to the data reader.

class DataSet {
    friend class UnitTest_DataSet;

 public:
    DataSet() = delete;
    DataSet(std::shared_ptr<IDataReader> reader);
    ~DataSet();

    DataSet(const DataSet& other) = delete;
    DataSet& operator=(const DataSet& other) = delete;

    std::size_t nFrames() const;

    std::size_t nRows() const; //!< The number of rows in each detector image
    std::size_t nCols() const; //!< The number of columns in each detector image

    InstrumentStateList& instrumentStates(); // TODO: rm if possible
    const InstrumentStateList& instrumentStates() const;

    void addMask(IMask* mask);
    void removeMask(IMask* mask);
    const std::set<IMask*>& masks() const;

    void maskPeaks(std::vector<Peak3D*>& peaks) const;

    //! Returns the intensity at point x,y,z.
    int dataAt(const std::size_t x = 0, const std::size_t y = 0, const std::size_t z = 0) const;

    //! Read a single frame
    Eigen::MatrixXi frame(const std::size_t idx) const;

    //! Returns frame after transforming to account for detector gain and baseline
    Eigen::MatrixXd transformedFrame(std::size_t idx) const;

    //! Gets the file handle.
    void open();

    //! Close file and release handle
    void close();

    //! Returns the sample-space q vector corresponding to a detector event
    ReciprocalVector computeQ(const DetectorEvent& ev) const;

    //! Returns the data reader used to set this dataset
    const IDataReader* reader() const;
    IDataReader* reader();

    const Detector& detector() const;

    std::string name() const;
    void setName(std::string name);

    //! Returns a const reference to the MetaData container
    const nsx::MetaData& metadata() const;

    //! Returns a reference to the MetaData container
    nsx::MetaData& metadata();

    //! Add a data file for reading data. Reading frames will be done only upon request.
    void addDataFile(const std::string& filename, const std::string& extension);

    //! Add a raw file to be read as a single detector image frame. Reading frames will be done only upon request.
    void addRawFrame(const std::string& rawfilename,
                     const RawDataReaderParameters* const params = nullptr);


private:
    void DataSet::_setReader(const DataFormat dataformat, const std::string& filename);

 private:
    std::string _name = nsx::kw_datasetDefaultName;
    unsigned int _nFrames;
    unsigned int _nrows;
    unsigned int _ncols;
    std::vector<Eigen::MatrixXi> _data;
    InstrumentStateList _states;
    std::set<IMask*> _masks;
    nsx::MetaData _metadata;
    //! Current data reader (set only once)
    std::shared_ptr<IDataReader> _reader;
    //! Current data format (set only once)
    DataFormat _dataformat = DataFormat::Unknown;
};

} // namespace nsx

#endif // NSX_CORE_DATA_DATASET_H
