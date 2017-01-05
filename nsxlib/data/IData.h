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

#ifndef NSXTOOL_IDATA_H_
#define NSXTOOL_IDATA_H_

#include <memory>
#include <string>
#include <set>

#include <Eigen/Dense>
#include "AABB.h"
#include "ComponentState.h"
#include "MetaData.h"
#include "Diffractometer.h"
#include "Peak3D.h"
#include "PeakCalc.h"

#include <mutex>
#include <future>

//#include "IFrameIterator.h"

#include "ProgressHandler.h"

namespace SX
{

namespace Instrument
{
    class Component;
}

namespace Data
{

using RowMatrixi = Eigen::Matrix<int, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>;
using RowMatrixd = Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>;

using namespace SX::Crystal;
using namespace SX::Geometry;
using namespace SX::Instrument;

class IFrameIterator;
class ThreadedFrameIterator;
class BasicFrameIterator;

using FrameIteratorCallback = std::function<IFrameIterator*(IData*, int)>;

/*! \brief Interface for diffraction data
 *
 * Base interface for all diffraction data. IData handles the IO
 */
class IData
{
public:
    // Constructors and destructor

    /*! Construct a IData Object from a file on disk, and pointer to a diffractometer.
     *  @param inMemory: whether the file should be loaded in memory straight away or kept on disk
     */
    IData(const std::string& filename, std::shared_ptr<Diffractometer> instrument);

    //! Copy constructor
    IData(const IData& other) = default;

    //! Destructor
    virtual ~IData()=0;

    // Operators
    //! Assignment operator
    IData& operator=(const IData& other);

    // iterators
    std::unique_ptr<IFrameIterator> getIterator(int idx);
    void setIteratorCallback(FrameIteratorCallback callback);

    // Getters and setters

    //! Gets the data basename
    std::string getBasename() const;

    //! Gets the data filename
    const std::string& getFilename() const;

    //! Gets a shared pointer to the diffractometer used to collect the data
    std::shared_ptr<Diffractometer> getDiffractometer() const;

    //! Return the number of frames
    std::size_t getNFrames() const;
    std::size_t getNRows() const;
    std::size_t getNCols() const;

    //! Gets a reference to the metadata of the data
    MetaData* getMetadata() const;

    //! Return the peaks
    std::set<sptrPeak3D>& getPeaks();

    //! Gets the interpolated state between two consecutive detector states
    ComponentState getDetectorInterpolatedState(double frame);

    //! Gets the the detector states.
    const ComponentState& getDetectorState(unsigned long frame) const;

    //! Gets the the detector states.
    const std::vector<ComponentState>& getDetectorStates() const;

    //! Get the sample state for frame
    const ComponentState& getSampleState(unsigned long frame) const;

    //! Gets the interpolated state between two consecutive sample states
    ComponentState getSampleInterpolatedState(double frame);

    //! Gets the the sample states.
    const std::vector<ComponentState>& getSampleStates() const;

    //! Get the source state for frame
    const ComponentState& getSourceState(unsigned int frame) const;

    //! Gets the interpolated state between two consecutive source states
    ComponentState getSourceInterpolatedState(double frame);

    //! Gets the the source states.
    const std::vector<ComponentState>& getSourceStates() const;

    //! Add a new mask to the data
    void addMask(AABB<double,3>* mask);

    //! Add a new peak to the data
    void addPeak(sptrPeak3D peak);

    //! Remove a mask from the data, by reference
    void removeMask(AABB<double, 3>* mask);

    //! Return the list of masks
    const std::set<AABB<double,3>*>& getMasks();

    //! Remove a peak from the data
    bool removePeak(sptrPeak3D peak);

    //! Clear the peaks collected for this data
    void clearPeaks();

    //! Return true if the file is stored in memory
    bool isInMemory() const;

    //! Load all the frames in memory
    void readInMemory(std::shared_ptr<SX::Utils::ProgressHandler> progress);

    //! Release the data from memory
    void releaseMemory();

    //! Return true if a given point (in detector space) belong to a mask
    bool inMasked(const Eigen::Vector3d& point) const;

    //! Mask the peaks collected in the data with the masks defined up to now
    void maskPeaks() const;

    //! Mask a given peak
    void maskPeak(sptrPeak3D peak) const;

    //! Return the intensity at point x,y,z.
    int dataAt(unsigned int x=0, unsigned int y=0, unsigned int z=0);

    //! Read a given Frame of the data
    Eigen::MatrixXi getFrame(std::size_t i);

    //! Read a single frame
    virtual Eigen::MatrixXi readFrame(std::size_t idx)=0;

    //! Get the file handle. Necessary to call before readInMemory or any IO of data.
    virtual void open()=0;

    //! Close file and release handle
    virtual void close()=0;

    //! True if file is open
    bool isOpened() const;

    //!
    std::size_t getFileSize() const;//
    void saveHDF5(const std::string& filename); // const;

    //! Is the peak h,k,l in Bragg condition in this dataset. Return Peak pointer if true,
    //! otherwise nullptr.
    std::vector<PeakCalc> hasPeaks(const std::vector<Eigen::Vector3d>& hkls,const Eigen::Matrix3d& BU);

    //! Get background
    double getBackgroundLevel(std::shared_ptr<SX::Utils::ProgressHandler> progress);

    //! Integrate intensities of all peaks
    void integratePeaks(std::shared_ptr<SX::Utils::ProgressHandler> handler = nullptr);

protected:
    bool _isOpened;
    std::string _filename;
    std::size_t _nFrames;
    std::size_t _nrows;
    std::size_t _ncols;
    std::shared_ptr<Diffractometer> _diffractometer;
    std::unique_ptr<MetaData> _metadata;
    bool _inMemory;
    std::vector<Eigen::MatrixXi> _data;
    std::vector<ComponentState> _detectorStates;
    std::vector<ComponentState> _sampleStates;
    std::vector<ComponentState> _sourceStates;
    std::set<sptrPeak3D> _peaks;
    std::size_t _fileSize;
    //! The set of masks bound to the data
    std::set<AABB<double,3>*> _masks;
    double _background;
    bool _isCached;
    FrameIteratorCallback _iteratorCallback;
};

} // end namespace Data

} // end namespace SX


#endif // NSXTOOL_IDATA_H_
