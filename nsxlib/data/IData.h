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

#ifndef NSXTOOL_DATA_H_
#define NSXTOOL_DATA_H_

#include <memory>
#include <string>
#include <set>

#include <Eigen/Dense>

#include "../crystal/Peak3D.h"
#include "../crystal/PeakCalc.h"
#include "../geometry/AABB.h"
#include "../instrument/Component.h"
#include "../instrument/Diffractometer.h"
#include "../instrument/InstrumentState.h"
#include "MetaData.h"

#include <mutex>
#include <future>

//#include "IFrameIterator.h"

#include "../utils/ProgressHandler.h"

namespace SX {
namespace Instrument {
    class Component;
}

namespace Data {

using RowMatrixi = Eigen::Matrix<int, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>;
using RowMatrixd = Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>;

using SX::Instrument::Diffractometer;
using SX::Instrument::Component;
using SX::Instrument::ComponentState;
using SX::Crystal::sptrPeak3D;
using SX::Geometry::AABB;
using SX::Crystal::PeakCalc;


class IFrameIterator;
class ThreadedFrameIterator;
class BasicFrameIterator;
class IDataReader;

using FrameIteratorCallback = std::function<IFrameIterator*(DataSet&, int)>;

/*! \brief Interface for diffraction data
 *
 * Base interface for all diffraction data. IData handles the IO
 */
class DataSet {
public:
    // Constructors and destructor

    /*! Construct a IData Object from a file on disk, and pointer to a diffractometer.
     */
    DataSet(IDataReader* reader, const std::shared_ptr<Diffractometer>& diffractometer);

    //! Copy constructor
    DataSet(const DataSet& other) = default;

    //! Destructor
    virtual ~DataSet();

    // Operators
    //! Assignment operator
    DataSet& operator=(const DataSet& other);

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

    //! Gets the the detector states.
    const ComponentState& getDetectorState(size_t frame) const;

    //! Get the sample state for frame
    const ComponentState& getSampleState(size_t frame) const;

    const ComponentState& getSourceState(size_t frame) const;

    //! Gets the the sample states
    const std::vector<SX::Instrument::InstrumentState>& getInstrumentStates() const;

    //! Get the interpolated state of a given component
    SX::Instrument::InstrumentState getInterpolatedState(double frame) const;
    //ComponentState getInterpolatedState(std::shared_ptr<Component> component, double frame) const;

    //! Add a new mask to the data
    void addMask(AABB<double,3>* mask);

    //! Add a new peak to the data
    void addPeak(const sptrPeak3D& peak);

    //! Remove a mask from the data, by reference
    void removeMask(AABB<double, 3>* mask);

    //! Return the list of masks
    const std::set<AABB<double,3>*>& getMasks();

    //! Remove a peak from the data
    bool removePeak(const sptrPeak3D& peak);

    //! Clear the peaks collected for this data
    void clearPeaks();

    //! Return true if a given point (in detector space) belong to a mask
    bool inMasked(const Eigen::Vector3d& point) const;

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
    std::vector<PeakCalc> hasPeaks(const std::vector<Eigen::Vector3d>& hkls,const Eigen::Matrix3d& BU);

    //! Get background
    double getBackgroundLevel(const std::shared_ptr<SX::Utils::ProgressHandler>& progress);

    //! Integrate intensities of all peaks
    void integratePeaks(const std::shared_ptr<SX::Utils::ProgressHandler>& handler = nullptr);

protected:
    bool _isOpened;
    std::string _filename;
    std::size_t _nFrames;
    std::size_t _nrows;
    std::size_t _ncols;
    std::shared_ptr<Diffractometer> _diffractometer;
    std::unique_ptr<MetaData> _metadata;
    std::vector<Eigen::MatrixXi> _data;
    std::vector<SX::Instrument::InstrumentState> _states;
    std::set<sptrPeak3D> _peaks;
    std::size_t _fileSize;
    //! The set of masks bound to the data
    std::set<AABB<double,3>*> _masks;
    double _background;
    FrameIteratorCallback _iteratorCallback;
    std::unique_ptr<IDataReader> _reader;
};

} // end namespace Data
} // end namespace SX

#endif // NSXTOOL_DATA_H_
