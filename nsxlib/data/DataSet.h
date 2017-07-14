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

#ifndef NSXLIB_DATASET_H
#define NSXLIB_DATASET_H

#include <mutex>
#include <string>
#include <set>
#include <memory>

#include <Eigen/Dense>

#include "../crystal/CrystalTypes.h"
#include "../data/DataTypes.h"
#include "../geometry/GeometryTypes.h"
#include "../geometry/IMask.h"
#include "../instrument/InstrumentTypes.h"
#include "../utils/UtilsTypes.h"

namespace nsx {

class DataSet {

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
    uptrIFrameIterator getIterator(int idx);
    #endif
    void setIteratorCallback(FrameIteratorCallback callback);

    // Getters and setters

    //! Gets the data basename
    std::string getBasename() const;

    //! Gets the data filename
    const std::string& getFilename() const;

    //! Gets a shared pointer to the diffractometer used to collect the data
    sptrDiffractometer getDiffractometer() const;

    //! Return the number of frames
    std::size_t getNFrames() const;
    std::size_t getNRows() const;
    std::size_t getNCols() const;

    //! Gets a reference to the metadata of the data
    MetaData* getMetadata() const;

    //! Return the peaks
    PeakSet& getPeaks();

    //! Gets the the detector states.
    const ComponentState& getDetectorState(size_t frame) const;

    //! Get the sample state for frame
    const ComponentState& getSampleState(size_t frame) const;

    const ComponentState& getSourceState(size_t frame) const;

    //! Gets the the sample states
    const InstrumentStateList& getInstrumentStates() const;

    //! Get the interpolated state of a given component
    InstrumentState getInterpolatedState(double frame) const;
    //ComponentState getInterpolatedState(std::shared_ptr<Component> component, double frame) const;

    //! Add a new mask to the data
    void addMask(IMask* mask);

    //! Add a new peak to the data
    void addPeak(const sptrPeak3D& peak);

    //! Remove a mask from the data, by reference
    void removeMask(IMask* mask);

    //! Return the list of masks
    const std::set<IMask*>& getMasks();

    //! Remove a peak from the data
    bool removePeak(const sptrPeak3D& peak);

    //! Clear the peaks collected for this data
    void clearPeaks();

    //! Mask the peaks collected in the data with the masks defined up to now
    void maskPeaks() const;

    //! Mask a given peak
    void maskPeak(sptrPeak3D peak) const;

    //! Return the intensity at point x,y,z.
    int dataAt(unsigned int x=0, unsigned int y=0, unsigned int z=0);

    //! Read a single frame
    Eigen::MatrixXi getFrame(std::size_t idx);

    //! Get the file handle.
    void open();

    //! Close file and release handle
    void close();

    //! True if file is open
    bool isOpened() const;

    //!
    std::size_t getFileSize() const;//
    void saveHDF5(const std::string& filename); // const;

    //! Is the peak h,k,l in Bragg condition in this dataset. Return Peak pointer if true,
    //! otherwise nullptr.
    PeakCalcList hasPeaks(const std::vector<Eigen::Vector3d>& hkls,const Eigen::Matrix3d& BU);

    //! Get background
    double getBackgroundLevel(const sptrProgressHandler& progress);

    //! Integrate intensities of all peaks
    void integratePeaks(double peak_scale = 3.0, double bkg_scale = 5.0, bool update_shape = false, const sptrProgressHandler& handler = nullptr);

    //! Remove duplicates
    void removeDuplicatePeaks();

    double getSampleStepSize() const;

    //! Get the q vector corresponding to a detector pixel
    Eigen::Vector3d getQ(const Eigen::Vector3d& pix) const;

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
    PeakSet _peaks;
    std::size_t _fileSize;
    //! The set of masks bound to the data
    std::set<IMask*> _masks;
    double _background;
    FrameIteratorCallback _iteratorCallback;
    std::shared_ptr<IDataReader> _reader;
};

} // end namespace nsx

#endif // NSXLIB_DATASET_H
