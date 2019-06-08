//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/data/DataSet.h
//! @brief     Defines ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef CORE_DATA_DATASET_H
#define CORE_DATA_DATASET_H

#include <memory>
#include <mutex>
#include <set>
#include <string>

#include <Eigen/Dense>

#include "core/crystal/CrystalTypes.h"
#include "core/data/DataTypes.h"
#include "core/geometry/GeometryTypes.h"
#include "core/geometry/IMask.h"
#include "core/instrument/InstrumentTypes.h"
#include "core/instrument/InterpolatedState.h"
#include "core/utils/UtilsTypes.h"

namespace nsx {

//! \brief Class used to manage loading detector images and metadata from disk.
class DataSet {
    friend class UnitTest_DataSet;

public:
    DataSet() = delete;

    DataSet(const DataSet& other) = default;

    //! Construct using the given data reader (allowing multiple formats)
    DataSet(std::shared_ptr<IDataReader> reader);
    ~DataSet();
    DataSet& operator=(const DataSet& other) = delete;

    //! Gets the data filename
    const std::string& filename() const;

    //! Returns the number of frames
    std::size_t nFrames() const;

    //! Returns the number of rows in each detector image
    std::size_t nRows() const;

    //! Returns the number of columns in each detector image
    std::size_t nCols() const;

    //! Gets the the sample states
    InstrumentStateList& instrumentStates();

    //! Gets the the sample states
    const InstrumentStateList& instrumentStates() const;

    //! Gets the interpolated state of a given component
    InterpolatedState interpolatedState(double frame) const;

    //! Add a new mask to the data
    void addMask(IMask* mask);

    //! Remove a mask from the data, by reference
    void removeMask(IMask* mask);

    //! Returns the list of masks
    const std::set<IMask*>& masks();

    //! Mask a given peak
    void maskPeaks(PeakList& peaks) const;

    //! Returns the intensity at point x,y,z.
    int dataAt(unsigned int x = 0, unsigned int y = 0, unsigned int z = 0);

    //! Read a single frame
    Eigen::MatrixXi frame(std::size_t idx);

    //! Returns frame after transforming to account for detector gain and baseline
    Eigen::MatrixXd transformedFrame(std::size_t idx);

    //! Returns a convolved frame
    Eigen::MatrixXd convolvedFrame(
        std::size_t idx, const std::string& convolver_type,
        const std::map<std::string, double>& parameters);

    //! Gets the file handle.
    void open();

    //! Close file and release handle
    void close();

    //! True if file is open
    bool isOpened() const;

    //! Returns total size of file
    std::size_t fileSize() const;

    //! Export dataset to HDF5 format
    void saveHDF5(const std::string& filename);

    //! Returns detector events corresponding to the list of q values.
    std::vector<DetectorEvent> events(const std::vector<ReciprocalVector>& sample_qs) const;

    //! Returns the sample-space q vector corresponding to a detector event
    ReciprocalVector computeQ(const DetectorEvent& ev) const;

    //! Returns the data reader used to set this dataset
    std::shared_ptr<IDataReader> reader() const;

private:
    bool _isOpened;

    std::string _filename;

    unsigned int _nFrames;

    unsigned int _nrows;

    unsigned int _ncols;

    std::vector<Eigen::MatrixXi> _data;

    InstrumentStateList _states;

    std::size_t _fileSize;

    //! The set of masks bound to the data
    std::set<IMask*> _masks;

    double _background;

    FrameIteratorCallback _iteratorCallback;

    std::shared_ptr<IDataReader> _reader;
};

} // end namespace nsx

#endif // CORE_DATA_DATASET_H
