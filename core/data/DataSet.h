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
#include "core/raw/IDataReader.h"

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

    const std::string& filename() const;
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
    int dataAt(unsigned int x = 0, unsigned int y = 0, unsigned int z = 0) const;

    //! Read a single frame
    Eigen::MatrixXi frame(std::size_t idx) const;

    //! Returns frame after transforming to account for detector gain and baseline
    Eigen::MatrixXd transformedFrame(std::size_t idx) const;

    //! Gets the file handle.
    void open();

    //! Close file and release handle
    void close();

    bool isOpened() const; //!< True if file is open

    std::size_t fileSize() const;

    void saveHDF5(const std::string& filename);

    //! Returns the sample-space q vector corresponding to a detector event
    ReciprocalVector computeQ(const DetectorEvent& ev) const;

    //! Returns the data reader used to set this dataset
    const IDataReader* reader() const;
    IDataReader* reader();

    const Detector& detector() const;

    std::string name() const;
    void setName(std::string name);

    //! List of data sources (filenames)
    std::vector<std::string> sources;

    //! Returns a const reference to the MetaData container
    const nsx::MetaData& metadata() const;

    //! Returns a reference to the MetaData container
    nsx::MetaData& metadata();

 private:
    bool _isOpened;
    std::string _filename;
    std::string _name;
    unsigned int _nFrames;
    unsigned int _nrows;
    unsigned int _ncols;
    std::vector<Eigen::MatrixXi> _data;
    InstrumentStateList _states;
    std::size_t _fileSize;
    std::set<IMask*> _masks;
    double _background;
    std::shared_ptr<IDataReader> _reader;
    nsx::MetaData _metadata;
};

} // namespace nsx

#endif // NSX_CORE_DATA_DATASET_H
