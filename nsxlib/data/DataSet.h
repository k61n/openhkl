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

#include "DataTypes.h"
#include "GeometryTypes.h"
#include "IMask.h"
#include "InstrumentTypes.h"
#include "PeakList.h"
#include "UtilsTypes.h"

namespace nsx {

class DataSet {
    friend class UnitTest_DataSet;

public:

    // Constructors and destructor

    /*! Construct a IData Object from a file on disk, and pointer to a diffractometer.
     */
    DataSet(std::shared_ptr<IDataReader> reader, const sptrDiffractometer& diffractometer);


    //! Copy constructor
    //DataSet(const DataSet& other) = default;

    //! Destructor
    virtual ~DataSet();

    // Operators
    //! Assignment operator
    DataSet& operator=(const DataSet& other);

    // iterators
    #ifndef SWIG
    uptrIFrameIterator iterator(int idx);
    #endif
    void setIteratorCallback(FrameIteratorCallback callback);

    //! Gets the data filename
    const std::string& filename() const;

    //! Gets a shared pointer to the diffractometer used to collect the data
    sptrDiffractometer diffractometer() const;

    //! Return the number of frames
    std::size_t nFrames() const;
    std::size_t nRows() const;
    std::size_t nCols() const;

    //! Gets a reference to the metadata of the data
    MetaData* metadata() const;

    //! Gets the the sample states
    InstrumentStateList& instrumentStates();

    //! Gets the the sample states
    const InstrumentStateList& instrumentStates() const;

    //! Get the interpolated state of a given component
    InstrumentState interpolatedState(double frame) const;
    //ComponentState getInterpolatedState(std::shared_ptr<Component> component, double frame) const;

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

    //! Get the file handle.
    void open();

    //! Close file and release handle
    void close();

    //! True if file is open
    bool isOpened() const;

    //!
    std::size_t fileSize() const;//
    void saveHDF5(const std::string& filename);
    
    //! Get background
    double backgroundLevel(const sptrProgressHandler& progress);

    //! Integrate intensities of all peaks
    void integratePeaks(const PeakList& peaks, double bkg_begin = 5.0, double bkg_end = 10.0, const sptrProgressHandler& handler = nullptr);

    //! Remove duplicates
    void removeDuplicatePeaks(nsx::PeakList& peaks);

    // todo: replace this with Jacobian of (x,y,f) -> q
    //double getSampleStepSize() const;      


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
