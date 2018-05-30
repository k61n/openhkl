/*
 * nsxtool : Neutron Single Crystal analysis toolkit
    ------------------------------------------------------------------------------------------
    Copyright (C)
    2012- Laurent C. Chapon, Eric C. Pellegrini Institut Laue-Langevin
    BP 156
    6, rue Jules Horowitz
    38042 Grenoble Cedex 9
    France
    chapon[at]ill.fr
    pellegrini[at]ill.fr

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#pragma once

#include <memory>
#include <mutex>
#include <set>
#include <string>

#include <Eigen/Dense>

#include "CrystalTypes.h"
#include "DataTypes.h"
#include "FitProfile.h"
#include "GeometryTypes.h"
#include "IMask.h"
#include "InstrumentTypes.h"
#include "InterpolatedState.h"
#include "UtilsTypes.h"

namespace nsx {

class DataSet {
    friend class UnitTest_DataSet;
public:
    //! Construct using the given data reader (allowing multiple formats) and diffractometer
    DataSet(std::shared_ptr<IDataReader> reader, const sptrDiffractometer& diffractometer);
    //! Destructor
    virtual ~DataSet();
    //! Assignment operator
    DataSet& operator=(const DataSet& other);
    //! Gets the data filename
    const std::string& filename() const;
    //! Gets a shared pointer to the diffractometer used to collect the data
    sptrDiffractometer diffractometer() const;
    //! Return the number of frames
    std::size_t nFrames() const;
    //! Return the number of rows in each detector image
    std::size_t nRows() const;
    //! Return the number of columns in each detector image
    std::size_t nCols() const;
    //! Gets a reference to the metadata of the data
    MetaData* metadata() const;
    //! Gets the the sample states
    InstrumentStateList& instrumentStates();
    //! Gets the the sample states
    const InstrumentStateList& instrumentStates() const;
    //! Get the interpolated state of a given component
    InterpolatedState interpolatedState(double frame) const;
    //! Add a new mask to the data
    void addMask(IMask* mask);
    //! Remove a mask from the data, by reference
    void removeMask(IMask* mask);
    //! Return the list of masks
    const std::set<IMask*>& masks();
    //! Mask a given peak
    void maskPeaks(PeakList& peaks) const;
    //! Return the intensity at point x,y,z.
    int dataAt(unsigned int x=0, unsigned int y=0, unsigned int z=0);
    //! Read a single frame
    Eigen::MatrixXi frame(std::size_t idx);
    //! Return frame after transforming to account for detector gain and baseline
    Eigen::MatrixXd transformedFrame(std::size_t idx);
    //! Return a convolved frame
    Eigen::MatrixXd convolvedFrame(std::size_t idx, const std::string& convolver_type, const std::map<std::string,double>& parameters);
    //! Get the file handle.
    void open();
    //! Close file and release handle
    void close();
    //! True if file is open
    bool isOpened() const;
    //! Return total size of file
    std::size_t fileSize() const;
    void saveHDF5(const std::string& filename);
    //! Return detector events corresponding to the list of q values.  
    std::vector<DetectorEvent> getEvents(const std::vector<ReciprocalVector>& sample_qs) const;
    //! Return the sample-space q vector corresponding to a detector event
    ReciprocalVector computeQ(const DetectorEvent& ev) const;

protected:
    bool _isOpened;
    std::string _filename;
    std::size_t _nFrames;
    std::size_t _nrows;
    std::size_t _ncols;
    sptrDiffractometer _diffractometer;
    uptrMetaData _metadata;
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
