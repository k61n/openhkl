#ifndef NSXLIB_DATA_H_
#define NSXLIB_DATA_H_

#include <future>
#include <memory>
#include <mutex>
#include <set>
#include <vector>

#include <Eigen/Dense>

#include "../crystal/Peak3D.h"
#include "../crystal/PeakCalc.h"
#include "../data/IDataReader.h"
#include "../geometry/AABB.h"
#include "../instrument/Diffractometer.h"
#include "../instrument/InstrumentState.h"
#include "../utils/ProgressHandler.h"

namespace SX {

namespace Data {

class IFrameIterator;
class ThreadedFrameIterator;
class BasicFrameIterator;

using FrameIteratorCallback = std::function<IFrameIterator*(Data&, int)>;

using SX::Crystal::sptrPeak3D;
using SX::Crystal::PeakCalc;
using SX::Geometry::AABB;
using SX::Instrument::Diffractometer;
using SX::Instrument::InstrumentState;

class Data {

public:
    Data(IDataReader& dataReader);

    ~Data();

    // iterators
    std::unique_ptr<IFrameIterator> getIterator(int idx);
    void setIteratorCallback(FrameIteratorCallback callback);

    //! Return the intensity at point x,y,z.
    int dataAt(size_t x=0, size_t y=0, size_t z=0);

    //! Return the peaks
    std::set<sptrPeak3D>& getPeaks();
    //! Clear the peaks collected for this data
    void clearPeaks();
    //! Remove a peak from the data
    bool removePeak(const sptrPeak3D& peak);
    //! Add a new peak to the data
    void addPeak(const sptrPeak3D& peak);
    //! Is the peak h,k,l in Bragg condition in this dataset. Return Peak pointer if true, otherwise nullptr.
    std::vector<PeakCalc> hasPeaks(const std::vector<Eigen::Vector3d>& hkls,const Eigen::Matrix3d& BU);

    //! Add a new mask to the data
    void addMask(AABB<double,3>* mask);
    //! Remove a mask from the data, by reference
    void removeMask(AABB<double, 3>* mask);
    //! Return the list of masks
    const std::set<AABB<double,3>*>& getMasks();
    //! Return true if a given point (in detector space) belong to a mask
    bool inMasked(const Eigen::Vector3d& point) const;
    //! Mask the peaks collected in the data with the masks defined up to now
    void maskPeaks() const;
    //! Mask a given peak
    void maskPeak(sptrPeak3D peak) const;

    //! Get background
    double getBackgroundLevel(const std::shared_ptr<SX::Utils::ProgressHandler>& progress);

    //! Integrate intensities of all peaks
    void integratePeaks(const std::shared_ptr<SX::Utils::ProgressHandler>& handler = nullptr);

    const IDataReader& getDataReader() const;

private:

    IDataReader& _dataReader;

    std::set<sptrPeak3D> _peaks;

    std::set<AABB<double,3>*> _masks;

    double _background;

    FrameIteratorCallback _iteratorCallback;

};

} // end namespace Data

} // end namespace SX

#endif /* NSXLIB_DATA_H_ */
